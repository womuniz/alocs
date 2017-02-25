import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;

public class InterfaceJava extends ConnectZoo{
    
    private static String rootphysic="/Meta/Fisica";
    private static ClusterMappingEstructure meta;
    
    public static String listServers() throws KeeperException, InterruptedException{
        
        String servers;
        
        zkInstance = connectZk();
        
        meta = new ClusterMappingEstructure();
        servers = meta.listServersDirectoriesBuckets(rootphysic);
        
        closeZk();
        
        return servers;
    
    }

    public static String listDirectories(String server) throws KeeperException, InterruptedException{
	
        String diretories;
        
        zkInstance = connectZk();
        
        meta = new ClusterMappingEstructure();
        diretories = meta.listServersDirectoriesBuckets(rootphysic+server);
        
        closeZk();
        
        return diretories;
    
    }

    public static String listBuckets(String path) throws KeeperException, InterruptedException{
	
        String buckets = "";
        
        zkInstance = connectZk();
        
        meta = new ClusterMappingEstructure();
        buckets = meta.listServersDirectoriesBuckets(rootphysic+path);
            
        closeZk();
        
        return buckets;
    
    }
    
    public static void createBucket(String path, int min, int max) throws KeeperException, InterruptedException, UnsupportedEncodingException, Exception{
        
        zkInstance = connectZk();
        
        meta = new ClusterMappingEstructure();
        meta.createBucket(path, min, max);
        
        closeZk();
    }

    public static void createDirectory(String path) throws KeeperException, InterruptedException{
        
        zkInstance = connectZk();
        
        meta = new ClusterMappingEstructure();
        meta.createDirectory(path);
        
        closeZk();
    }

    public static void createServer(String path) throws KeeperException, InterruptedException{
        
        zkInstance = connectZk();
        
        if(zkInstance != null){
            meta = new ClusterMappingEstructure();
            meta.createServer(path);
        }else
            System.out.println("Nao conectado ao zookeeper");
            
        closeZk();
    }
    
    public static String getMetadata(String path) throws KeeperException, InterruptedException, UnsupportedEncodingException{
        
        String metadata;
        
        zkInstance = connectZk();
        
        meta = new ClusterMappingEstructure();
        metadata = meta.getMetadata(path);
        
        closeZk();
        
        return metadata;
    }
    
    public static String locateInterval(String interval) throws KeeperException, InterruptedException, Exception{
        
        IntervalTree tree;
        String location = ""; 
        
        zkInstance = connectZk();
        
        tree = new IntervalTree();
        tree.locateInterval(interval);
        
        if(location == null)
            System.out.println("Not found!!");
        
        closeZk();
        
        return location;
    }
    
    public static String fetchKey(int key) throws KeeperException, InterruptedException, Exception{
        
        IntervalTree tree;
        String location;
        
        zkInstance = connectZk();
        
        tree = new IntervalTree();
        location = tree.fetchKey(key);
        
        closeZk();
        
        return location;
    }
    
    public static void deleteBucket(String path) throws InterruptedException, UnsupportedEncodingException, Exception{
        
        ClusterMappingEstructure map;
        
        zkInstance = connectZk();
        
        map = new ClusterMappingEstructure();
        map.deleteBucket(path);
        
        closeZk();
    }
    
    public static void rebalancing() throws KeeperException{
        
        IntervalTree tree;
        
        zkInstance = connectZk();
        
        tree = new IntervalTree();
        
        try {
            tree.rebalancing();
        } catch (InterruptedException ex) {
            //Logger.getLogger(InterfaceJava.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        closeZk();
        
    }
}

