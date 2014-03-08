#if 0
void doIt(int a, int b){
    int i,j, inv;
    for(i=0;;i++){
        for(j=0;;j++){
            inv = a +b;
            if(j<b) break;        
        }
        if(i<b) break;
    }
}

void main(){
    doit();

}

int g;
int g_incr (int c)
{
    g += c;
}

int loop(int a, int b, int c)
{
    int i;
    int ret = 0;
    for(i = a; i < b; i++) {
        g_incr(c);
    }
    return ret + g;
}


int loop2(int a, int b, int c)
{
    int i;
    int v;
    int ret = 0;
    for(i = a; i < b; i++) {

        int j = 0;
        ret = 3 * i;
        j = ret - 1;
        v = g_incr(j);
    }

    printf("%i", v);
    return ret + g;
}

int loop3(int a, int b, int c)
{
    int i;
    int v;
    int ret = 0;

    for(i = a; i < b; i++) {
        if( i * c > 0 )
        {
            ret = i * ret;
        }
        else
        {
            ret = i - c;
        }

        print("%i",ret);

    }


    return ret + g;
}

#endif

int myloop(int k){

    goto l1;

    int a = 0; int b = 12; int c =12;
l1: 
    if(k<100)
        b = 12;
    else 
        b = 123;

    for(; ; a++)
    {
        int inv;
        inv = b+7;
        if(a<k) break;
    }

}

#if(0)
#endif

int loop4(int a, int b, int c)
{

    for(; ; a++)
    {
        int inv;
        inv = c + b;
        if(a<b) break;
        //       printf("%i, %i",a,inv);
    }
    printf("%i",c);
    return c;
}


int loop5(int a, int b, int c)
{
    int inv;

    for(; ; a++)
    {
        inv = c + b;
        if(a<b) break;
    }

    loop4(a,b,c);

    return inv;
}

