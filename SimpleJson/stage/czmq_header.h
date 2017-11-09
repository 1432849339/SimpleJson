/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-8-28 16:05
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_CZMQ_HEADER_H_
#define ISON_BASE_CZMQ_HEADER_H_

//  Opaque class structures to allow forward references
typedef struct _zactor_t zactor_t;
typedef struct _zauth_t zauth_t;
typedef struct _zbeacon_t zbeacon_t;
typedef struct _zcert_t zcert_t;
typedef struct _zcertstore_t zcertstore_t;
typedef struct _zchunk_t zchunk_t;
typedef struct _zconfig_t zconfig_t;
typedef struct _zctx_t zctx_t;
typedef struct _zdigest_t zdigest_t;
typedef struct _zdir_t zdir_t;
typedef struct _zdir_patch_t zdir_patch_t;
typedef struct _zfile_t zfile_t;
typedef struct _zframe_t zframe_t;
typedef struct _zhash_t zhash_t;
typedef struct _zlist_t zlist_t;
typedef struct _zlog_t zlog_t;
typedef struct _zloop_t zloop_t;
typedef struct _zmonitor_t zmonitor_t;
typedef struct _zmsg_t zmsg_t;
typedef struct _zmutex_t zmutex_t;
typedef struct _zpoller_t zpoller_t;
typedef struct _zproxy_t zproxy_t;
typedef struct _zrex_t zrex_t;
typedef struct _zsock_t zsock_t;
typedef struct _zsock_monitor_t zsock_monitor_t;
typedef struct _zuuid_t zuuid_t;

struct zmq_pollitem_t;

#endif // ISON_BASE_CZMQ_HEADER_H_