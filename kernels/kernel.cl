kernel void imageProc(global int* v, global int* v2){
    unsigned int i = get_global_id(0) * 3;
    unsigned step = v2[0];
    unsigned sz = v2[1];
    unsigned int sum[3] = {v[sz+i], v[sz+i+1], v[sz+i+2]};
    unsigned int cnt = 1;
    if((int)(i - step) >= 0){
        sum[0] += v[sz+i-step];
        sum[1] += v[sz+i+1-step];
        sum[2] += v[sz+i+2-step];
        cnt++;
    }
    if((int)(i - step - 3) >= 0){
        sum[0] += v[sz+i-step-3];
        sum[1] += v[sz+i+1-step-3];
        sum[2] += v[sz+i+2-step-3];
        cnt++;
    }
    if((int)(i - step + 3) >= 0){
        sum[0] += v[sz+i-step+3];
        sum[1] += v[sz+i+1-step+3];
        sum[2] += v[sz+i+2-step+3];
        cnt++;
    }
    if((int)(i + step) < sz){
        sum[0] += v[sz+i+step];
        sum[1] += v[sz+i+1+step];
        sum[2] += v[sz+i+2+step];
        cnt++;
    }
    if((int)(i + step - 3) < sz){
        sum[0] += v[sz+i+step-3];
        sum[1] += v[sz+i+1+step-3];
        sum[2] += v[sz+i+2+step-3];
        cnt++;
    }
    if((int)(i + step + 3) < sz){
        sum[0] += v[sz+i+step+3];
        sum[1] += v[sz+i+1+step+3];
        sum[2] += v[sz+i+2+step+3];
        cnt++;
    }
    if((int)(i - 3) >= 0){
        sum[0] += v[sz+i-3];
        sum[1] += v[sz+i+1-3];
        sum[2] += v[sz+i+2-3];
        cnt++;
    }
    if((int)(i + 3) < sz){
        sum[0] += v[sz+i+3];
        sum[1] += v[sz+i+1+3];
        sum[2] += v[sz+i+2+3];
        cnt++;
    }
    v[i] = sum[0] / cnt;
    v[i+1] = sum[1] / cnt;
    v[i+2] = sum[2] / cnt;
}