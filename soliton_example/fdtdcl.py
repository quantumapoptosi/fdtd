import numpy as np
import os
import pyopencl as cl
import sys

def nothing(object):
    """
    Nothing returns nothing. 
    """
    return None

class ComputeNode:
    
    def __init__(self,platform_number=None,device_number=None):
        self.platform_number = platform_number
        self.device_number = device_number
        self.platform = self.get_platform()
        self.device = self.get_device()
        self.context = cl.Context([self.device])
        self.queue = cl.CommandQueue(self.context)
    
    def get_platform(self):
        """
        Determine the platform based on input.
        """
        platforms = cl.get_platforms()
        if self.platform_number is None: 
            print('\nERROR: You must choose a platform_number related to the platform (machine/computer) that you wish to use.' 
            'The following options are detected:\n')
            print('     \n'.join('{}: {}'.format(*k) for k in enumerate(platforms)))
            print('\n')
            sys.exit()
        else:
            return platforms[self.platform_number]
            
    def get_device(self):
        """Determine the device based on input.
        """
        devices = self.platform.get_devices()
        if self.device_number is None: 
            print('\nERROR: You must choose a device_number related to the device (CPU/GPU) that you wish to use.' 
            'The following options are detected:\n')
            print('     \n'.join('{}: {}'.format(*k) for k in enumerate(devices)))
            print('\n')
        else:
            return devices[self.device_number]

class Field:
    
    def __init__(self, compute_node, init_vals_real, init_vals_imag, prg, update_function = nothing, is_potential="no",multistep_modifier=0):
        self.compute_node = compute_node
        self.multistep = len(init_vals_real)
        self.init_vals_real = np.float64(init_vals_real) #multistepxJxK array of real fxn values @ points evaluated at initial time steps (n=0,1,...)
        self.init_vals_imag = np.float64(init_vals_imag) #multistepxJxK array of imag fxn values @ points evaluated at initial time steps (n=0,1,...)
        self.shape = np.append([2*self.multistep + multistep_modifier],self.init_vals_real[0].shape)
        self.values_real = self.setup_array_real() #set up multistepxJxK array
        self.values_imag = self.setup_array_imag() #set up multistepxJxK array
        self.values_real_dev = self.move_to_device(self.values_real) #load real values onto the device (CPU/GPU)
        self.values_imag_dev = self.move_to_device(self.values_imag) #load imag values onto the device (CPU/GPU)
        self.is_potential=(is_potential=='potential') #does this field describe a potential? 
        self.prg = prg #program for Field
        self.update_function = update_function
        self.thestep = 0
        
    def setup_array_real(self):
        """Sets up up array of values to use in computation.
        """
        array = np.zeros(self.shape) #array of zeros with new shape
        for element in range(self.multistep):
            array[element]=self.init_vals_real[element]
        return array
    
    def setup_array_imag(self):
        """Sets up up array of values to use in computation.
        """
        
        array = np.zeros(self.shape) #array of zeros with new shape
        for element in range(self.multistep):
            array[element]=self.init_vals_imag[element]
        return array
            
    def move_to_device(self, array):
        """Moves values from Memory to the Device Memory Buffer.
        """
        ctx = self.compute_node.context
        array_dev = cl.Buffer(ctx, cl.mem_flags.READ_WRITE | cl.mem_flags.COPY_HOST_PTR, hostbuf=array)
        return array_dev
        
    def field_to_device(self):
        """
        Moves both real and imaginary parts to device. 
        """
        self.values_real_dev = self.move_to_device(self.values_real)
        self.values_imag_dev = self.move_to_device(self.values_imag)
        return None
    
    def take_from_device(self,array_dev, array):
        """Moves values from Device Memory Buffer to Memory.
        """
        queue = self.compute_node.queue
        cl.enqueue_copy(queue, array_dev, array)
        return None
        
    def field_from_device(self):
        """Moves values from Device Memory Buffer to Memory.
        """
        self.take_from_device(self.values_real,self.values_real_dev)
        self.take_from_device(self.values_imag,self.values_imag_dev)
        
    def update(self,simulationtime=None):
        """Contruct the update function to update the wave
        a single time step.
        """
        self.update_function(self)
        self.thestep = self.thestep + 1
        
class Simulation:
    
    def __init__(self,field_list,num_temps=None,**kwargs):
        self.fields = field_list #list of fields
        self.waves = [field for field in self.fields if field.is_potential==False] #list of waves
        self.potentials = [field for field in self.fields if field.is_potential==True] #list of potentials
        self.time_step = 0;
        self.dt = kwargs.get('dt',1)
        if "h" in kwargs: #If h is passed, then set all values to be the same.
            self.dx = kwargs.get('h',1)
            self.dy = kwargs.get('h',1)
            self.dz = kwargs.get('h',1)
        else: #otherwise, set all values that exist.
            if "dx" in kwargs:
                self.dx = kwargs.get('dx',1)
            if "dy" in kwargs:
                self.dy = kwargs.get('dy',1)
            if "dz" in kwargs: 
                self.dz = kwargs.get('dz',1)
        return        
        
    def update(self):
        for field in self.fields: 
            field.update()
            self.time_step = self.time_step + 1
        
            
class CLProgram:
    """
    double = False will inable single digit precision. 
    """
    def __init__(self, context, file_name, **kwargs):
        self.context = context
        self.file_name = file_name
        self.kernel_source = self.get_kernel_source()
        self.global_vars = kwargs.get('global_vars','')
        if 'double' in kwargs:
            if kwargs.get('double',True) == True:
                self.double_macro = '#pragma OPENCL EXTENSION cl_khr_fp64 : enable;'
            if kwargs.get('double',False) == False:
                self.double_macro = ''
        else:
            self.double_macro = '#pragma OPENCL EXTENSION cl_khr_fp64 : enable' 
        self.program = cl.Program(self.context, self.double_macro+self.global_vars+self.kernel_source)
        #self.program = self.Program.build()
        #self.kernal_names = self.Program.get_info('DEVICES')
        
    def get_kernel_source(self):
        with open(self.file_name, "r") as file_name:
            codestring = file_name.readlines()
        nullstring = ""
        codestring = nullstring.join(codestring).replace("\n","")
        return "\n"+codestring 
    
            
    
    
        
    
        
        
        
        
        
    
    
        
        
