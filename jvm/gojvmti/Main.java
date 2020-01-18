import java.util.Arrays;
import java.lang.reflect.Method;
import java.util.*;

public class Main {
  public static void count(String str){
  }
  public Integer loop(int m){
    //Integer ii = new Integer(0);
    Integer ii = Integer.valueOf(0);
    HashMap<Integer, String> hash_map = new HashMap<Integer, String>();
    for (int i=0;i<m;i++){
      Integer k = Integer.valueOf(i);
      String v = String.valueOf(i);
      hash_map.put(k,v);
      int[] arr = new int[100];
      ii+=k;
    }
    return ii;
  }
  public static void main(String[] args) throws Exception{
    int max = Integer.parseInt(args[0]);
    Integer r = new Main().loop(max);
    System.out.println("Final result= "+r);
/*
    String methodName = "toString";
    try{
      Method m = Object.class.getDeclaredMethod(methodName);
      byte[] bytecode = getByteCodes(m);
      System.out.println(Arrays.toString(bytecode));
    }catch(NoSuchMethodException e){
      System.out.println("No such method: "+methodName);
    }catch(Error e){
      e.printStackTrace();
    }
*/
  }

  //private static native int countInstances(Class klass);
  //private static native int countAllInstances();
  //Java_Main_getBytecodes
  //private static native byte[] getByteCodes(Method method);
  
}
