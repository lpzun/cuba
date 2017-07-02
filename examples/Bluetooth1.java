package thread;

/**
 * Bluetooth driver version 1.
 * 
 * @author suwimont
 *
 */
public class Bluetooth1 {

	public static void BCSP_PnpAdd(Device d) {
		int status = BCSP_IoIncrement(d);
		if (status == 0)
			assert(!d.stopped);
		BCSP_IoDecrement(d);
	}
	
	public static void BCSP_PnpStop(Device d) {
		d.stoppingFlag = true;
		BCSP_IoDecrement(d);
		while (!d.stoppingEvent) {}
		d.stopped = true;	// Switch#2 (stopped = true, stoppingFlag = true, pendingIo = 0, stoppingEvent = true)
	}
	
	public static int BCSP_IoIncrement(Device d) {
		if (d.stoppingFlag)
			return -1;	// Switch#1 (stoppingFlag = false, pendingIo = 1, stoppingEvent = false)
		synchronized(d) {
			d.pendingIo++;
		}
		return 0;
	}
	
	public static void BCSP_IoDecrement(Device d) {
		int pendingIo;
		synchronized (d) {
			d.pendingIo--;
			pendingIo = d.pendingIo;
		}
		if (pendingIo == 0) {
			d.stoppingEvent = true;
		}
	}
	
	/**
	 * A jMoped test method
	 * 
	 * Suggested parameters: 5/20/2/3
	 * 
	 * Result: assertion failed in BCSP_PnpAdd()
	 */
	static void test() {
		final Device d = new Device();
		new Thread(new Runnable() {
			public void run() {
				Bluetooth1.BCSP_PnpStop(d);
			}
		}).start();
		Bluetooth1.BCSP_PnpAdd(d);
	}
}
