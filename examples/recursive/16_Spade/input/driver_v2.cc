class DEVICE_EXTENSION {
   bool stopped;
   bool driverStoppingFlag;
   bool stoppingEvent;
   int  pendingIo;
};
  class DEVICE_OBJECT
{
   DEVICE_EXTENSION deviceExtension;
};

class BlueTooth
{
   static void BCSP_Miniport_PnpStop (DEVICE_OBJECT BtDevice)
   {
     DEVICE_EXTENSION deviceExtension = BtDevice.deviceExtension;
     deviceExtension.driverStoppingFlag = true;
     BCSP_IoDecrement(deviceExtension);
     KeWaitForStoppingEvent(deviceExtension);
     deviceExtension.stopped = true;
   }

   static void BCSP_Miniport_PnpAdd (DEVICE_OBJECT BtDevice)
   {
     DEVICE_EXTENSION deviceExtension = BtDevice.deviceExtension;
     int status;
     status = BCSP_IoIncrement (deviceExtension);
     if(status > 0)
     {
       // do work here
       assert(!deviceExtension.stopped);
     }

     BCSP_IoDecrement(deviceExtension);
   }


   static int BCSP_IoIncrement(DEVICE_EXTENSION e)
   {
     int status;
     InterlockedIncrementPendingIo(e);
     if(e.driverStoppingFlag == true){
         BCSP_IoDecrement(e);
        status = -1;
     }
     else {
        status = 1;
     }
     return(status);
   }

  static void BCSP_IoDecrement(DEVICE_EXTENSION e)
   {
     int pendingIo;
     pendingIo = InterlockedDecrementPendingIo(e);
      if(pendingIo == 0)
       KeSetEventStoppingEvent(e);
   }

   static atomic  int InterlockedIncrementPendingIo(DEVICE_EXTENSION e)
   {
     e.pendingIo = e.pendingIo + 1;
     return(e.pendingIo);
   }

   static atomic int InterlockedDecrementPendingIo(DEVICE_EXTENSION e)
   {
     e.pendingIo = e.pendingIo - 1;
     return(e.pendingIo);
   }
      static void KeSetEventStoppingEvent(DEVICE_EXTENSION e)
   {
     e.stoppingEvent = true;
   }

   static void KeWaitForStoppingEvent(DEVICE_EXTENSION e)
   {
     select { wait (e.stoppingEvent) -> ; }
   }

   activate static void main(){
     DEVICE_OBJECT device = new DEVICE_OBJECT;
     device.deviceExtension = new DEVICE_EXTENSION;
     device.deviceExtension.pendingIo = 1;
     device.deviceExtension.stoppingEvent = false;
     async BCSP_Miniport_PnpAdd (device);
     async BCSP_Miniport_PnpStop(device);
   }
};

