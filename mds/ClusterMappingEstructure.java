import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooDefs;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.data.Stat;

public class ClusterMappingEstructure extends ConnectZoo{

    private Auxiliary aux;
    private IntervalTree val;
    private IntervalTree stree;
    private final String rootphysic = "/Meta/Fisica";
    
    
    public ClusterMappingEstructure(){
        
        this.aux = new Auxiliary();
        this.val = new IntervalTree();
        this.stree = new IntervalTree();
    }
    
    public String listServersDirectoriesBuckets(String node) throws KeeperException, InterruptedException{
        Stat stat = zkInstance.exists(node, true);
        List<String> children;  
        
        if (stat == null) {
            System.out.println("Path doesn't exist!!");
            return null;
        }
        
        children = zkInstance.getChildren(node, false); 
        
        return children.toString().replace("[", "").replace("]", "").replace(",", " -");         
    }
    
    public void createServer(String server) throws KeeperException, InterruptedException{
        
        if(zkInstance != null){
            Stat staServ = zkInstance.exists(rootphysic+"/"+server, true);
            
            if (staServ == null) {
                byte[] serverData = aux.encodeServerData();
                server=rootphysic+"/"+server;
                
                try {
                    zkInstance.create(server, serverData, ZooDefs.Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
                    System.out.println("** Server created! **");
                } catch (InterruptedException ex) {
                    //Logger.getLogger(ClusterMappingEstructure.class.getName()).log(Level.SEVERE, null, ex);
                }
            }else
                System.out.println("Server already exists!");
        }else
            System.out.println("Instancia do Zookeeper nao iniciada!");    
    }
    
    public void createDirectory(String directory) throws KeeperException, InterruptedException{
	
        System.out.println("IMDS->Create Dir -> : "+directory);      	
        String extrem[] = directory.split("/"), server=extrem[0];
        Stat staServ = zkInstance.exists(rootphysic+"/"+server, true);
        Stat staDir = zkInstance.exists(rootphysic+"/"+directory, true);
        
        if (staServ == null)
            createServer(server);                         
        if(staDir == null){
            byte[] directoryData = aux.encodeDirectoryData();
            try { 
                directory = rootphysic+"/"+directory;
                zkInstance.create(directory, directoryData, ZooDefs.Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
                System.out.println("** Directory created! **");
            } catch (InterruptedException ex) {
                //Logger.getLogger(ClusterMappingEstructure.class.getName()).log(Level.SEVERE, null, ex);
            }
        }else{
            System.out.println("Server already exists!");
        }    
        
    }
    
    public void createBucket(String path, int min, int max) throws KeeperException, InterruptedException, UnsupportedEncodingException, Exception{
	       
        String in = String.valueOf(min)+"-"+String.valueOf(max);
        byte[] data = in.getBytes();
        String interval = Integer.toString(min)+"-"+Integer.toString(max); 
        String location = path;
        String extrem[] = path.split("/"), server=extrem[0], directory=extrem[1];
        
        //criar servidor se o mesmo nao existir
        Stat staServ = zkInstance.exists(rootphysic+"/"+server, true);
        if (staServ == null) {
            createServer(server);              
        }  
        
        //criar diretorio se o mesmo nao existir
        Stat staDir = zkInstance.exists(rootphysic+"/"+server+"/"+directory, true);
        if (staDir == null) {
            directory= server+"/"+directory; 
            createDirectory(directory);
        }
        
        //criar o bucket
        path=rootphysic+"/"+path;            
        try {             
            zkInstance.create(path, data, ZooDefs.Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
            stree.putInterval(location,interval, min, max);
            System.out.println("** Bucket created! **");
        } catch (KeeperException | InterruptedException e) {
            System.out.println(e.getMessage());
        }
        
    }
    
    public String getMetadata(String path) throws KeeperException, InterruptedException, UnsupportedEncodingException{
        String data = aux.getMetadata(rootphysic+path);
        
        if(data != null){
            System.out.println("No associated data found\n");
            return null;
        }else 
            return data;
    }
    
    public void deleteBucket(String path) throws KeeperException, InterruptedException, UnsupportedEncodingException, Exception{
        Stat stat = zkInstance.exists(rootphysic+path, true);
        
        if (stat != null) {
            String interval = aux.getMetadata(rootphysic+path);
            String location = val.locateInterval(interval);
            if(location!=null){
                val.cleanInterval(location);
                zkInstance.delete(rootphysic+path, stat.getVersion());
            } 
        }else
            System.out.println("Bucket doesn't exist!!");
    }
}


