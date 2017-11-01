/*
 struct DEVICE_EXTENSION {
	int pendingIo;
	bool stoppingFlag;
	bool stoppingEvent;
};

bool stopped;
void main() {
	DEVICE_EXTENSION *e = malloc(sizeof(DEVICE_EXTENSION));
	e->pendingIo = 1;
	e->stoppingFlag = false;
	e->stoppingEvent = false;
	stopped = false;
	async BCSP_PnpStop( e);
	BCSP_PnpAdd(e);
}
void BCSP_PnpAdd(DEVICE_EXTENSION *e) {
	int status;
	status = BCSP_IoIncrement(e);
	if (status == 0) {
		// do work here
		assert
		!stopped;
	}
	BCSP_IoDecrement(e);
}
void BCSP_PnpStop(DEVICE_EXTENSION *e) {
	e->stoppingFlag = true;
	BCSP_IoDecrement(e);
	assume e
	->stoppingEvent;
	// release allocated resources
	stopped = true;
}
int BCSP_IoIncrement(DEVICE_EXTENSION *e) {
	if (e->stoppingFlag)
		return -1;
	atomic
	{
		e->pendingIo = e->pendingIo + 1;
	}
	return 0;
}
void BCSP_IoDecrement(DEVICE_EXTENSION *e) {
	int pendingIo;
	atomic
	{
		e->pendingIo = e->pendingIo - 1;
		pendingIo = e->pendingIo;
	}
	if (pendingIo == 0)
		e->stoppingEvent = true;
}
 */
