kernel void imageProc(global int* v, global int* v2){
    unsigned int i = get_global_id(0) * 3;
    unsigned int sum[3] = {v[v2[1]+i], v[v2[1]+i+1], v[v2[1]+i+2]};
    unsigned int cnt = 1;
    for(unsigned b = 0; b < 10; b++){
        if((int)(i - b*v2[0]) >= 0){
            sum[0] += v[v2[1]+i-b*v2[0]];
            sum[1] += v[v2[1]+i+1-b*v2[0]];
            sum[2] += v[v2[1]+i+2-b*v2[0]];
            cnt++;
        }
        if((int)(i - b*v2[0] - 3) >= 0){
            sum[0] += v[v2[1]+i-b*v2[0]-3];
            sum[1] += v[v2[1]+i+1-b*v2[0]-3];
            sum[2] += v[v2[1]+i+2-b*v2[0]-3];
            cnt++;
        }
        if((int)(i - b*v2[0] + 3) >= 0){
            sum[0] += v[v2[1]+i-b*v2[0]+3];
            sum[1] += v[v2[1]+i+1-b*v2[0]+3];
            sum[2] += v[v2[1]+i+2-b*v2[0]+3];
            cnt++;
        }
        if((int)(i + b*v2[0]) < v2[1]){
            sum[0] += v[v2[1]+i+b*v2[0]];
            sum[1] += v[v2[1]+i+1+b*v2[0]];
            sum[2] += v[v2[1]+i+2+b*v2[0]];
            cnt++;
        }
        if((int)(i + b*v2[0] - 3) < v2[1]){
            sum[0] += v[v2[1]+i+b*v2[0]-3];
            sum[1] += v[v2[1]+i+1+b*v2[0]-3];
            sum[2] += v[v2[1]+i+2+b*v2[0]-3];
            cnt++;
        }
        if((int)(i + b*v2[0] + 3) < v2[1]){
            sum[0] += v[v2[1]+i+b*v2[0]+3];
            sum[1] += v[v2[1]+i+1+b*v2[0]+3];
            sum[2] += v[v2[1]+i+2+b*v2[0]+3];
            cnt++;
        }
    }
    v[i] = sum[0] / cnt;
    v[i+1] = sum[1] / cnt;
    v[i+2] = sum[2] / cnt;
}