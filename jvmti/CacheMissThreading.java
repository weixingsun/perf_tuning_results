import java.util.*;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;
import java.util.concurrent.*;

public class CacheMissThreading {
  public static void main(String[] args) throws InterruptedException, ExecutionException {
    List<Integer> intList = new ArrayList<>();
    for (int i = 0; i < 10000000; i++) {
      intList.add(i);
    }
    runTestSingle(intList,5);
    runTestThreading(intList,5);
  }
  private static void runTestSingle(List<Integer> l, int n){
    for (int i=0; i<n; i++){
      doFunTask(l);
    }
  }
  private static void runTestThreading(List<Integer> l, int n){
    List<CompletableFuture<Void>> callbacks = new ArrayList<>();
    for (int i=0; i<n; i++){
      callbacks.add(threading(l, i));
    }
    
    try{
      for (CompletableFuture<Void> f:callbacks){
        f.get();
      }
    }
    catch(InterruptedException ie){}
    catch(ExecutionException ee){}
    //TimeUnit.MINUTES.sleep(1);
  }
  private static CompletableFuture<Void> threading(List<Integer> intList, int t) {
     return CompletableFuture.runAsync(() -> {
      Thread.currentThread().setName("Thread - "+t);
      System.out.println(Thread.currentThread().getName()+ " created");
      doFunTask(intList);
    });
    
  }

  private static void doFunTask(List<Integer> intList) {
    long startTime = System.currentTimeMillis();
    intList.stream().map(i -> i * 2).collect(Collectors.toList());
    long endTime = System.currentTimeMillis();
    System.out.println( "Thread : " + Thread.currentThread().getName() + " : Time Taken in (ms) : " + (endTime - startTime));
  }
}

/*
Thread : main : Time Taken in (ms) : 1075
Thread : main : Time Taken in (ms) : 1156
Thread : main : Time Taken in (ms) : 1610
Thread : main : Time Taken in (ms) : 867
Thread : main : Time Taken in (ms) : 1517
Thread - 1 created
Thread - 2 created
Thread - 0 created
Thread - 3 created
Thread - 4 created
Thread : Thread - 1 : Time Taken in (ms) : 3318
Thread : Thread - 2 : Time Taken in (ms) : 3319
Thread : Thread - 0 : Time Taken in (ms) : 4494
Thread : Thread - 3 : Time Taken in (ms) : 3815
Thread : Thread - 4 : Time Taken in (ms) : 3819
*/