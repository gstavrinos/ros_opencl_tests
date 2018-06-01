kernel void imageProc(global char* v, global char* v2){
    unsigned int i = get_global_id(0);
    // TODO more complicated example
    if (abs(v[i] - v2[i]) < 20) {
        v[i] = 0;
    }
}