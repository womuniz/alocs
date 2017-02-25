import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.util.Scanner;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.data.Stat;

public class ConnectZoo {
    
	protected static ZooKeeper zkInstance;
  
	public static ZooKeeper connectZk() {
    
    try{
      //zookeeper esta inicializado no ceph-mon
      zkInstance = new ZooKeeper("192.168.10.16:2181",12000, null);
    }catch(IOException e){
      System.out.println(e.getMessage());  
    } 
  
    return zkInstance;  
  }
	
	public static void closeZk() {
		try{ 
			zkInstance.close();
    }catch (InterruptedException e){
			System.out.println(e.getMessage());
    }
	}

}