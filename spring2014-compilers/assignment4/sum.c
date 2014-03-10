#if(0)
void doIt(int a, int b){
    int i,j, inv;
    for(i=0;;i++){
        for(j=0;;j++){
            inv = a +b; //2 hoist
            if(j<b) break;        
        }
        if(i<b) break;
    }
}

/*void main(){
    doit();
}
*/

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


    return ret + c;
}


/*
 Following loop has same invariant instruction inside nested loops.
 Both the instructions should be lifted and inv = b + 7 should be before inv = b + 3
 */

int myloop1(int k){

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


int myloop2(int k){

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




int loop7(int a, int b, int c)
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


int loop9(int a, int b, int c)
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

///////////////////////////////////////////
///// TEST CASE SET = 2 || RUN THEM SEPARATLEY
/////////////////////////////////////////

#if(0)

int mySimpleCase(int k){
    int i,j, inv;
    for(i=0;;i++){
        inv = k * 2323;  //should be hoisted
        if(i<k) break;
    }
    return inv;
}

//hoisting count = 1

int mySimpleCase2(int k){
    int i,j, inv;
    for(j=0;;j++){
        for(i=0;;i++){
            inv = k * 2323;  //should be hoisted
            if(i<k) break;
        }
        if(j<k) break;
    }
    return inv;
}

//hoisting count = 3 one for each loop

int mySimpleCase3(int k){
    int i,j, inv, ran;
    for(j=0;;j++){
        for(i=0;;i++){
            inv = k * 2323;  //should be hoisted
            if(i<k) break;
        }
        ran = inv + k; //should be hoisted
        if(j<k) break;
    }
    return inv;
}

//hoisting count = 3 + 3 = 6 //again one for each loop + 1 loop

int mySimpleCase4(int k){
    int i,j, inv, inv2, ran;
    for(j=0;;j++){
        for(i=0;;i++){
            inv = k * 2323;  //should be hoisted levels = 2
            inv2 = inv + k; //shoud be hoisted levels = 2
            if(i<k) break;
        }
        ran = inv + k; //should be hoisted level = 1
        if(j<k) break;
    }
    return inv;
}

//hoisting count = 6 + 5 = 11 //again one for each loop + 1 


int mySimpleCase5(int k){
    int i,j, inv, inv2, ran;
    for(j=0;;j++){
        for(i=0;;i++){
            inv = k * 2323;  //should be hoisted levels = 1
            inv2 = inv + k; //shoud be hoisted levels = 1
            if(i<k) break;
        }
        ran = inv + k; //should not be hoisted
        k = inv2 + 24;
        if(j<k) break;
    }
    return inv;
}

//hoisting count = 13
#endif

int mySimpleCase6(int k){
    int i,j, inv, inv2, ran;
    for(j=0;;j++){
        int l = 23;
        for(i=0;;i++){
            inv = k * 2323;  //should be hoisted levels = 2
            inv2 = inv + k; //shoud be hoisted levels = 2
            if(i<k) break;
        }
        ran = inv + l; //should be hoisted
        l = inv2 + 24; //should be hoisted
        if(j<k) break;
    }
    return inv;
}


//All instructions should be hoisted.
int mySimpleCase7(int k){
    int i,j, inv, inv2, ran;
    for(j=0;;j++){
        int l = 23;
        for(i=0;;i++){
            inv = l * 2323;  //should be hoisted levels = 2
            inv2 = inv + k; //shoud be hoisted levels = 2
            if(i<k) break;
        }
        ran = inv + l; //should be hoisted
        l = inv2 + 24; //should be hoisted
        if(j<k) break;
    }
    return inv;
}

void main(){
    int s = mySimpleCase7(3);
    int k = mySimpleCase6(4);

    printf("%d, %d", s, k);
}

