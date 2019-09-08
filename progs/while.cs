class Program
{
   static void Main()
   {
      int u = 50;
      while (u <= 55)
      {
          u = u + 1;
          if (u == 53)
          {
             break;
          }
      }
      print(u);
   }
}

