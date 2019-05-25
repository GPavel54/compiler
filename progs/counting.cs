class Program
{
    static void Main()
    {
		int u = 10;
        int r = u * 13 / 2;
        while (r >= 0)
        {
            r = r - 1;
            u = u + 10;
        }
        //Write(u);
        //r = myfunc(u);
    }

    int myfunc(int p1)
    {
        p1 = p1 + 40;
        return p1;
    }
}

