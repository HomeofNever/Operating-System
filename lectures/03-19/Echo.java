/**
 * Echo class to display given
 *  character c exactly n times
 *
 * Implements the Runnable interface
 *  so that multithreading can occur
 */
public class Echo implements Runnable /* or extends Thread */
{
  private char c;
  private int n;

  /** constructor to create an Echo object */
  public Echo( char c, int n )
  {
    this.c = c;
    this.n = n;
  }

  /**
   * Must provide an implementation (code) for
   * the run() method
   */
  public void run()
  {
    for ( int i = 0 ; i < n ; i++ )
    {
      System.out.print( "[" + c + "]" );
//      System.out.print( c );
    }

    System.out.println( "Echo (" + c + ") all done!" );
  }
}
