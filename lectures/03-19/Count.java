/**
 * Count class to display integers 0 to n
 *
 * Implements the Runnable interface
 *  so that multithreading can occur
 */
public class Count implements Runnable
{
  private int n;

  /** constructor to create a Count object */
  public Count( int n )
  {
    this.n = n;
  }

  /**
   * Must provide an implementation (code) for
   * the run() method
   */
  public void run()
  {
    for ( int i = 0 ; i <= n ; i++ )
    {
      System.out.print( "[" + i + "]" );
    }
  }
}
