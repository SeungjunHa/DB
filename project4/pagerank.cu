__device__ __constant__ int c_num_nodes;
__global__
void device_graph_propagate(const uint* graph_indices, 
        const uint* graph_edges, 
        const float* graph_nodes_in, 
        float* graph_nodes_out, 
        const float* inv_edges_per_node)
        //,int num_nodes) 
{

    // TODO: fill in the kernel code here
    int node_index = blockIdx.x*blockDim.x + threadIdx.x;
    if(node_index < c_num_nodes){
        float sum = 0.f;
        for(uint j = graph_indices[node_index]; j < graph_indices[node_index+1]; j++) {
            sum += graph_nodes_in[graph_edges[j]]*inv_edges_per_node[graph_edges[j]];
        }
        graph_nodes_out[node_index] = 0.5f/(float)c_num_nodes+0.5f*sum;
    }
}

double device_graph_iterate(const uint* h_graph_indices
        , const uint* h_graph_edges
        , const float* h_node_values_input
        , float* h_gpu_node_values_output
        , const float* h_inv_edges_per_node
        , int nr_iterations
        , int num_nodes
        , int avg_edges) {
    // TODO: allocate GPU memory
    int err_1 = cudaSuccess;
    int err_2 = cudaSuccess;
    int err_3 = cudaSuccess;
    int err_4 = cudaSuccess;
    int err_5 = cudaSuccess;
    //int err_6 = cudaSuccess;

    float *cuda_1;
    float *cuda_2;
    uint *cuda_graph_indices;
    uint *cuda_graph_edges;
    float *cuda_inv_edges_per_node;
    //int *cuda_num_nodes;

    err_1 = cudaMalloc((void **)&cuda_1, num_nodes*sizeof(float));
    err_2 = cudaMalloc((void **)&cuda_2, num_nodes*sizeof(float));
    err_3 = cudaMalloc((void **)&cuda_graph_indices, (num_nodes+1)*sizeof(uint));
    err_4 = cudaMalloc((void **)&cuda_graph_edges, (num_nodes*avg_edges)*sizeof(uint));
    err_5 = cudaMalloc((void **)&cuda_inv_edges_per_node, (num_nodes)*sizeof(float));
    //err_6 = cudaMalloc((void **)&cuda_num_nodes, sizeof(int));
    // TODO: check for allocation failure
    if(err_1) throw err_1;
    if(err_2) throw err_2;
    if(err_3) throw err_3;
    if(err_4) throw err_4;
    if(err_5) throw err_5;
    //if(err_6) throw err_6;

    // TODO: copy data to the GPU
    cudaMemcpy(cuda_1, h_node_values_input, num_nodes*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_2, h_node_values_input, num_nodes*sizeof(float), cudaMemcpyHostToDevice); 
    cudaMemcpy(cuda_graph_indices, h_graph_indices, (num_nodes+1)*sizeof(uint), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_graph_edges, h_graph_edges, (num_nodes*avg_edges)*sizeof(uint), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_inv_edges_per_node, h_inv_edges_per_node, (num_nodes)*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(c_num_nodes, &num_nodes, sizeof(int), 0, cudaMemcpyHostToDevice);
    //cudaMemcpy(cuda_num_nodes, &num_nodes, sizeof(int), cudaMemcpyHostToDevice);
    start_timer(&timer);

    const int thread_num = 1024; // Should be divisor of 32768.
    //GTX 1060, Maximum thread_num is 1024.
    int block_size = 512;//(num_nodes)/thread_num;

    // TODO: launch your kernels the appropriate number of iterations
    for(int iter = 0; iter < nr_iterations/2 ; iter++) {
            device_graph_propagate<<<block_size, thread_num>>>(cuda_graph_indices, cuda_graph_edges, cuda_1, cuda_2, cuda_inv_edges_per_node);//, num_nodes);
            device_graph_propagate<<<block_size, thread_num>>>(cuda_graph_indices, cuda_graph_edges, cuda_2, cuda_1, cuda_inv_edges_per_node);//, num_nodes);
    }

    check_launch("gpu graph propagate");
    double gpu_elapsed_time = stop_timer(&timer);
    // TODO: copy final data back to the host for correctness checking
    if(nr_iterations % 2) {
        device_graph_propagate<<<block_size, thread_num>>>(cuda_graph_indices, cuda_graph_edges, cuda_1, cuda_2, cuda_inv_edges_per_node);//, num_nodes);
        cudaMemcpy(h_gpu_node_values_output, cuda_2, num_nodes*sizeof(float), cudaMemcpyDeviceToHost);
    } else {
        cudaMemcpy(h_gpu_node_values_output, cuda_1, num_nodes*sizeof(float), cudaMemcpyDeviceToHost);
    }
    // TODO: free the memory you allocated!
    cudaDeviceSynchronize();
    cudaFree(cuda_1);
    cudaFree(cuda_2);
    cudaFree(cuda_graph_indices);
    cudaFree(cuda_graph_edges);
    cudaFree(cuda_inv_edges_per_node);

    return gpu_elapsed_time;
}
