/**
 * Threads.java creates a few Count and Echo objects/tasks
 *  and runs them as multiple threads
 */
public class Threads
{
  public static void main( String[] args )
  {
    // Create the "task" objects
    Count c1 = new Count( 10 );
    Echo e1 = new Echo( 'R', 10 );
    Echo e2 = new Echo( 'P', 10 );
    Echo e3 = new Echo( 'I', 10 );

    // TO DO: create a few more Count and Echo objects/threads

    // this is NOT the way to create multiple threads
//    e1.run();
//    e2.run();
//    e3.run();
//    c1.run();

    // Create the Thread objects
    Thread t1 = new Thread( c1 );
    Thread t2 = new Thread( e1 );
    Thread t3 = new Thread( e2 );
    Thread t4 = new Thread( e3 );

    // Start the threads
    t1.start();   // start() method here creates the
    t2.start();   //  thread, then calls the run() method
    t3.start();
    t4.start();

    System.out.println( "Joining in child threads....." );

    try
    {
      if ( t1.isAlive() )
      {
        t1.join();     // BLOCKS
      }
      System.out.println( "t1 thread terminated" );
      if ( t2.isAlive() )
      {
        t2.join();     // BLOCKS
      }
      System.out.println( "t2 thread terminated" );
      if ( t3.isAlive() )
      {
        t3.join();     // BLOCKS
      }
      System.out.println( "t3 thread terminated" );
      if ( t4.isAlive() )
      {
        t4.join();     // BLOCKS
      }
      System.out.println( "t4 thread terminated" );
    }
    catch ( InterruptedException ex )
    {
      System.err.println( "Received InterruptedException..." );
    }


  }
}
