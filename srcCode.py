def match(a,b,c):
    if(a==b):
        print("YES")
        a = b+3
    else:
        while(b<c):
            b = b+1
            match(a,b,c)
        print("NO")
x = 1
for r in range(1,100):
    x=x*r
    if(x<1000000007):
        x=x+1
    else:
        x=x%1000000007
    match(x,r,2)
print("COMPLETE")