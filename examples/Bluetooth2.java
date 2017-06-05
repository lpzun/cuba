package thread;

/**
 * Bluetooth version 2.
 * 
 * @author suwimont
 *
 */
public class Bluetooth2 {

	public static void BCSP_PnpAdd(Device device) {
		int status = BCSP_IoIncrement(device);
		if (status > 0) {
			assert(!device.stopped);
//			BCSP_IoDecrement(device); // Bug fix?
		}
		BCSP_IoDecrement(device);
	}
	
	public static void BCSP_PnpStop(Device device) {
		device.stoppingFlag = true;
		BCSP_IoDecrement(device);
		while (!device.stoppingEvent) {}
		device.stopped = true;
	}
	
	public static int BCSP_IoIncrement(Device device) {
		int status;
		synchronized(device) {
			device.pendingIo++;
		}
		if (device.stoppingFlag) {
			BCSP_IoDecrement(device);
			status = -1;
		} else {
			status = 1;
		}
		
		return status;
	}
	
	public static void BCSP_IoDecrement(Device device) {
		int pendingIo;
		synchronized (device) {
			device.pendingIo--;
			pendingIo = device.pendingIo;
		}
		if (pendingIo == 0) {
			device.stoppingEvent = true;
		}
	}
	
	/**
	 * A jMoped test method
	 * 
	 * Suggested parameters: 5/31/3/5
	 * 
	 * Result: assertion failed.
	 */
	static void test0() {
		final Device e = new Device();
		new Thread(new Runnable() {
			public void run() {
				Bluetooth2.BCSP_PnpAdd(e);
			}
		}).start();
		new Thread(new Runnable() {
			public void run() {
				Bluetooth2.BCSP_PnpStop(e);
			}
		}).start();
		Bluetooth2.BCSP_PnpAdd(e);
	}
	
	/**
	 * A jMoped test method
	 * 
	 * Suggested parameters: 5/31/3/4
	 * 
	 * Result: assertion failed
	 */
	static void test1() {
		final Device e = new Device();
		new Thread(new Runnable() {
			public void run() {
				Bluetooth2.BCSP_PnpStop(e);
			}
		}).start();
		new Thread(new Runnable() {
			public void run() {
				Bluetooth2.BCSP_PnpStop(e);
			}
		}).start();
		Bluetooth2.BCSP_PnpAdd(e);
	}
}
