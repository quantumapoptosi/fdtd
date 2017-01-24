inline int id(
    int n,
    int j,
    int k){
        int id_;
        return id_= j%WIDTH + (k%WIDTH) * WIDTH + n*WIDTH*WIDTH;
    };
    
inline double abs2(
    global double * u,
    global double * v,
    int n,
    int j,
    int k){
       return u[id(n,j,k)]*u[id(n,j,k)] + v[id(n,j,k)]*v[id(n,j,k)];
    };

inline double D_of(
    global double * u,
    int n,
    int j,
    int k){
           return -(u[id(n,j+2,k)]+u[id(n,j,k+2)]+u[id(n,j-2,k)]+u[id(n,j,k-2)]-16.0*u[id(n,j+1,k)]-16.0*u[id(n,j,k+1)]-16.0*u[id(n,j-1,k)]-16.0*u[id(n,j,k-1)]+60.0*u[id(n,j,k)])/12.0;
    };

inline double Dy_of(
    global double * u,
    int n,
    int j,
    int k){
           return  (u[id(n,j,k+1)]+u[id(n,j,k-1)]-2.0*u[id(n,j,k)]);
    };

inline double Dx_of(
    global double * u,
    int n,
    int j,
    int k){
           return (u[id(n,j+1,k)]+u[id(n,j-1,k)]-2.0*u[id(n,j,k)]);
    };

inline double D12y_of(
    global double * u,
    int n,
    int j,
    int k){
           return  -(u[id(n,j,k+2)]+u[id(n,j,k-2)]-16.0*u[id(n,j,k+1)]-16.0*u[id(n,j,k-1)]+30.0*u[id(n,j,k)])/12.0;
    };

inline double D12x_of(
    global double * u,
    int n,
    int j,
    int k){
           return -(u[id(n,j+2,k)]+u[id(n,j-2,k)]-16.0*u[id(n,j+1,k)]-16.0*u[id(n,j-1,k)]+30.0*u[id(n,j,k)])/12.0;
    };
    

    
inline bool region_total(
    int j,
    int k){
        if ( (j>=0) && (j<WIDTH) && (k>=0) && (k<WIDTH) ){
            return true;
        }
        else { 
            return false;
        }
    };

inline bool region_main(
    int j,
    int k){
        if ( (j>=(bnd1+bnd2)) && (j<(WIDTH-(bnd1+bnd2))) && (k>=(bnd1+bnd2)) && (k<(WIDTH-(bnd1+bnd2))) ){
            return true;
        }
        else { 
            return false;
        }
    };

inline bool region_north1(
    int j,
    int k){
        if ( (k>=(WIDTH-(bnd1+bnd2))) && (k<(WIDTH-bnd1)) && (j>(bnd1-1)) && (j<(WIDTH-bnd1)) ){
            return true;
        }
        else { 
            return false;
        }
    };

inline bool region_west1(
    int j,
    int k){
        if ( (j>(1)) && (j<(bnd1+bnd2)) && (k>(bnd1-1)) && (k<(WIDTH-bnd1)) ){
            return true;
        }
        else { 
            return false;
        }
    };

inline bool region_south1(
    int j,
    int k){
        if ( (k>(1)) && (k<(bnd1+bnd2)) && (j>(bnd1-1)) && (j<(WIDTH-bnd1)) ){
            return true;
        }
        else { 
            return false;
        }
    };

inline bool region_east1(
    int j,
    int k){
        if ( (j>=(WIDTH-(bnd1+bnd2))) && (j<(WIDTH-bnd1)) && (k>(bnd1-1)) && (k<(WIDTH-bnd1)) ){
            return true;
        }
        else { 
            return false;
        }
    };

inline bool region_east(
    int j,
    int k){
        if ( (j>=(WIDTH-bnd1)) && (j<(WIDTH)) && (k>(bnd1-1)) && (k<(WIDTH-bnd1)) ){
            return true;
        }
        else { 
            return false;
        }
    };

inline bool region_north(
    int j,
    int k){
        if ( (k>=(WIDTH-bnd1)) && (k<(WIDTH)) && (j>(bnd1-1)) && (j<(WIDTH-bnd1)) ){
            return true;
        }
        else { 
            return false;
        }
    };

inline bool region_south(
    int j,
    int k){
        if ( (k>=0) && (k<bnd1) && (j>(bnd1-1)) && (j<(WIDTH-bnd1)) ){
            return true;
        }
        else { 
            return false;
        }
    };

inline bool region_west(
    int j,
    int k){
        if ( (j>=0) && (j<bnd1) && (k>(bnd1-1)) && (k<(WIDTH-bnd1)) ){
            return true;
        }
        else { 
            return false;
        }
    };

inline bool region_main1(    
    int j,
    int k){
        if (region_south1(j,k)||region_north1(j,k)||region_west1(j,k)||region_east1(j,k)){
            return true;
        }
        else { 
            return false;
        }
    }

inline bool region_northeast(
    int j,
    int k){
        if ( (j>=(WIDTH-bnd1)) && (j<(WIDTH)) && (k>=(WIDTH-bnd1)) && (k<(WIDTH)) ){
            return true;
        }
        else { 
            return false;
        }
    };
inline bool region_southeast(
    int j,
    int k){
        if ( (j>=(WIDTH-bnd1)) && (j<(WIDTH)) && (k>=0) && (k<bnd1)  ){
            return true;
        }
        else { 
            return false;
        }
    };

inline bool region_northwest(
    int j,
    int k){
        if ( (j>=(0)) && (j<(bnd1))  && (k>=(WIDTH-bnd1)) && (k<(WIDTH)) ){
            return true;
        }
        else { 
            return false;
        }
    };
inline bool region_southwest(
    int j,
    int k){
        if ( (j>=(0)) && (j<(bnd1)) && (k>=0) && (k<bnd1)  ){
            return true;
        }
        else { 
            return false;
        }
    };

/*comment*/
__kernel void test(
        __global double * u, 
        __global double * v
        ){
    
        int gid_n = get_global_id(0);
        int gid_j = get_global_id(1);
        int gid_k = get_global_id(2);
        int n = gid_n;
        int j = gid_j % WIDTH;
        int k = gid_k % WIDTH;
        if(region_main(j,k)){
            u[id(n,j,k)] = id(n,j,k);
            v[id(n,j,k)] = id(n,j,k);
        }
        
        };

__kernel void solve2(
        __global double * u, 
        __global double * v
        ){

        int gid_n = get_global_id(0);
        int gid_j = get_global_id(1);
        int gid_k = get_global_id(2);
        int n = gid_n;
        int j = gid_j % WIDTH;
        int k = gid_k % WIDTH;

        u[id(n,j,k)] = j;
        v[id(n,j,k)] = k;

        };