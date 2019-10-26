public class Main {
  public static void main(String[] args) {
    System.out.println("Hello World!");
    Thread t = new Thread();
    int a = countInstances(Thread.class);
	System.out.println("There are " + a + " instances of " + Thread.class);
	System.out.println("DEBUT ALLOC");
	int is = countInstances(Integer.class);
    System.out.println("There are " + is + " instances of " + Integer.class);
	for (int i=0;i<100;i++){
      Integer ii = 25000;
	}
	int iis = countInstances(Integer.class);
    System.out.println("There are " + iis + " instances of " + Integer.class);
	String aa = new String("COUCOU");
	new java.util.ArrayList<String>();
	System.out.println("FIN ALLOC");
  }

  private static native int countInstances(Class klass);
}
