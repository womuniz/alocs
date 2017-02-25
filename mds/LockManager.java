import java.util.concurrent.TimeUnit;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooDefs;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.data.ACL;
import org.apache.zookeeper.recipes.lock.LockListener;
import org.apache.zookeeper.recipes.lock.WriteLock;

public class LockManager{

    private String lock_name;
    private String path;
    private WriteLock wLock;
    private CountDownLatch lockAcqSignal = new CountDownLatch(1);

    public static final List<ACL> DEFAULT_ACL = ZooDefs.Ids.OPEN_ACL_UNSAFE;

    public LockManager(String lock_name, ZooKeeper zk, String path) {
        this(lock_name, zk, path, DEFAULT_ACL);
    }

    public LockManager(String lock_name, ZooKeeper zk, String path, List<ACL> acl) {
        this.lock_name = name;
        this.path = path;
        this.wLock = new WriteLock(zk, path, acl, new SyncLockListener());
    }

    public void lock() throws InterruptedException, KeeperException {
        System.out.printf("%s requesting lock on %s...\n", _name, _path);
        wLock.lock();
        lockAcqSignal.await();
    }

    public boolean lock(long timeout, TimeUnit unit) throws InterruptedException, KeeperException {
        System.out.printf("%s requesting lock on %s with timeout %d %s...\n", _name, _path, timeout, unit.name());
        wLock.lock();
        return lockAcqSignal.await(timeout, unit);
    }

    public boolean tryLock() throws InterruptedException, KeeperException {
        return lock(1, TimeUnit.SECONDS);
    }

    public void unlock() {
        wLock.unlock();
    }

    class SyncLockListener implements LockListener {

        @Override
        public void lockAcquired() {
            System.out.printf("Lock acquired by %s on %s\n", lock_name, path);
            lockAcqSignal.countDown();
        }

        @Override
        public void lockReleased() {
            System.out.printf("Lock released by %s on %s\n", lock_name, path);
        }
    }
}    