#ifndef URL_h
#define URL_h



//const char serviceURL[] = "http://myradar.com.bd/api/receive-services";
const char serviceURL[] = "http://myradar.com.bd/api/device/consume/service";

//const char URL_ENGINE_GET_STATUS[] = "http://myradar.com.bd/api/CarLocked";

const char URL_ENGINE_UPDATE_STATUS[] = "http://myradar.com.bd/api/device/engine/update";

//const char URL_GEOFENCE_QUERY[] = "http://myradar.com.bd/api/device/fence/list";
//const char URL_GEOFENCE_STATUS_UPDATE[] = "http://myradar.com.bd/api/device/fence/update/status";


//const char URL_TIME[] = "http://myradar.com.bd/api/getTimer";

//const char URL_USSD_FORMAT[] = "http://myradar.com.bd/api/device/ussd/find";
const char URL_USSD_BALANCE[] = "http://myradar.com.bd/api/device/ussd/update";

// Combined API Setup
const char URL_SETUP_INIT[] = "http://myradar.com.bd/api/device/setup/init";


const char URL_DIAGNOSTICS[] = "http://myradar.com.bd/api/device/health/store";


// speed limit exceed notification
const char URL_SPEED_LIMIT[] = "http://myradar.com.bd/api/device/speed/notify";

const char URL_REMAINING_NIGHT[] = "http://myradar.com.bd/api/device/midnight/diff";

#endif // !URL_h
