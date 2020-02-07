import java.util.Arrays;
import java.lang.reflect.Method;
import java.util.*;

public class Main {
	/*static {
		System.loadLibrary("heap");
	}*/
	public static void count(int i, HashMap<Integer, String> map){
		Integer k = Integer.valueOf(i);
		String v = String.valueOf(i);
		map.put(k,v);
		int[] arr = new int[100];
		map.get(k);
	}
	public Integer loop(int m){
		int total = 0;
		HashMap<Integer, String> map = new HashMap<Integer, String>();
		for (int i=0;i<m;i++){
			count(i,map);
			total +=i;
		}
		return total;
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
    //public static native String getNativeString();
    public static native int add(int a, int b);
	private static int enabled = 0;
}
