class Program
{
    static void Main()
    {
        int yt[10];
	    int u = 10 * yt[3];
        int r = u * 13 / 2;
        char str[30];
        char t;
        char wtring[10] = "Hello";
        while (r >= 0)
        {
            r = r - 1;
            u = u + 10;
        }
        Write(u);
        //r = myfunc(u);
    }

    int myfunc(int p1)
    {
        p1 = p1 + 40;
        return p1;
    }

    void anotherFunc(int p1)
    {
        char t = 'a';
        int u = 23;
    }
}

