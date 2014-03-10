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


/*
 Following loop has same invariant instruction inside nested loops.
 Both the instructions should be lifted and inv = b + 7 should be before inv = b + 3
 */

int myloop(int k){

    goto l1;

    int a = 0; int b = 12; int c =12;
l1: 
    if(k<100)
        b = 12;
    else 
        b = 123;
    for(;;k++){
        int inv;
        for(; ; a++)
        {
            inv = b+7;
            if(a<k) break;
        }
        inv = b + 3;
        if(k<1000) break;
    }
    return 0;
}

#endif

#if(0)
int myloop(int k){

    if(k<12)
        goto l1;

    int a = 0; int b = 12; int c =12;

    if(k<100)
        b = 12;
    else 
        b = 123;

l1: 
    for(;;k++){
        int inv;
        for(; ; a++)
        {
            inv = b+7;
            if(a<k) break;
        }
        inv = b + 3;
        if(k<1000) break;
    }
    return 0;
}
#endif

#if(0)

//since l1 doesn't domintate l2. instructions should not be hoisted above l2
int myloop4(int k){

    if(k<12)
        goto l2;

    int a = 0; int b = 12; int c =12;

    if(k<100)
        b = 12;
    else 
        b = 123;

l1: 
    for(;;k++){
        int inv;
l2:    {

       }
       for(; ; a++)
       {
           inv = b+7;
           if(a<k) break;
       }
       inv = b + 3;
       if(k<1000) break;
    }
    return 0;
}

#endif

//inv should be hoisted
int l = 2434;
int myloop6(){
    for(int i = 0;;i++){
        int inv;
        int fd = 24;
        inv = fd * 2323;
        if(i<8) break;
    }
}



#if(0)

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

#endif
