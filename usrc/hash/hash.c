
unsigned long hash(unsigned char *str)
    {
unsigned int hash = 0;
int c;

while (c = *str++)
    hash += c^1;

return (hash%10);
    }


main()
{

 char *str1="alok";
 char *str2="shweta";
 char *str3 ="random1";
 char *str4 ="random2";
 int a[10];

printf("%d\n",hash(str1));
printf("%d\n",hash(str2));
printf("%d\n",hash(str3));
printf("%d\n",hash(str4));
 a[hash(str1)]=3101;
 a[hash(str2)]=4102;
 a[hash(str3)]=7103;
 a[hash(str4)]=6104;

 printf("Phone no of alok=%d",a[hash(str1)]);

}
