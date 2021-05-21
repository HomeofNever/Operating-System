/**
 * SumV2 attempts to take a divide-and-conquer
 *  approach to summing integers 1..1,000,000
 *
 * This time, we're going to synchronize the threads
 *  and therefore always get the correct answer
 */
public class SumV2
{
  // all child threads add to this global sum variable
  //  to accumulate the final sum 1..1,000,000
  private long sum;
  //           ^^^
  // we want to make sure that access to the global variable sum
  //  is synchronized, meaning that only ONE thread accesses/changes
  //   the sum variable at any given time
  private Object lock;
    // use this to achieve MUTUAL EXCLUSION


  public void doit()
  {
    sum = 0L;

    lock = new Object();  // create a "dummy object" that we
                          //  can lock/synchronize on...

    int n = 1000000;  // this must be evenly divisible by 100000

    SumThread[] t = new SumThread[ n / 100000 ];

    // divide the summation problem into many subtasks (threads)
    for ( int j = 0 ; j < t.length ; j++ )
    {
      t[j] = new SumThread( j * 100000 + 1, (j+1) * 100000 );
    }

            // t[0]         1 - 100,000
            // t[1]   100,001 - 200,000
            // t[2]   200,001 - 300,000
            //   etc.


    // start all of the threads
    for ( int j = 0 ; j < t.length ; j++ )
    {
      t[j].start();
    }


    // Join in child threads one by one; once all threads
    //  are done, we should have the sum fully calculated!
    try
    {
      int threads = t.length;

      // TO DO: this loop is called a "busy wait" loop,
      //         which is rather inefficient; change this
      //          to be more efficient ... Thread.sleep()
      while ( threads > 0 )
      {
        for ( int j = 0 ; j < t.length ; j++ )
        {
          if ( t[j] != null && !t[j].isAlive() )
          {
            // join the child thread back in to the main thread of execution
            t[j].join();
              // the above join() call is a BLOCKING call

            System.out.println( "MAIN: joined " + t[j].getName() );
            t[j] = null;
            threads--;
          }
        }
      }
    }
    catch ( InterruptedException ex )
    {
      System.err.println( "ERROR: thread interrupted" );
      System.exit( 1 );
    }

    System.out.println( "SUM 1.." + n + " IS " + sum );


    // calculate and show expected result -- CSCI 1100 style
    long calcSum = 0;

    for ( int i = 1 ; i <= n ; i++ )
    {
      calcSum += i;
    }

    System.out.println( "       (should be " + calcSum + ")" );
  }


  class SumThread extends Thread
  {
    private int m;    // sum values m..n
    private int n;

    public SumThread( int m, int n )
    {
      System.out.println( "ADDING " + m + ".." + n + " TO SUM" );
      this.m = m;
      this.n = n;
    }

    public void run()    // the start() method creates the thread,
    {                    //  then calls our run() method here...
      System.out.println( "RUNNING " + this.getName() );

      for ( int i = m ; i <= n ; i++ )
      {
        // the synchronized() block means that only one thread
        //  can be in this block of code at any given time
        synchronized( lock )
        {
          sum += i;   // compiler generates byte code:
                      //
                      // this will always be atomic:
                      //   load the sum variable
                      //   add i to it
                      //   store the result back in the sum variable
        }
      }
    }
  }


  public static void main( String[] args )
  {
    SumV2 a = new SumV2();
    a.doit();
  }
}
