/**
 *  test_drv.c
 *
 */


#include "osa.h"
#include "core.h"


static osa_err_t    _def_init(ats_tevent_t *tevent);
static ats_result_t _def_start_test(void *testCase);
static void         _def_stop_test(ats_tevent_t *tevent);
static void         _def_suss_cb(ats_tevent_t *tevent);
static void         _def_fail_cb(ats_tevent_t *tevent);
static void         _def_fini(ats_tevent_t *tevent);
static void			_def_trv_remove(ats_tdrv_t *tevent);


ats_tdrv_t  *ats_tdrv_new(const osa_char_t *drv_file)
{
    ats_tdrv_t *drv = (ats_tdrv_t *)osa_mem_alloc(sizeof(ats_tdrv_t));

    drv->dev = NULL;
    strncpy(drv->drv_file, drv_file, OSA_NAME_MAX - 1);
    osa_list_init(&drv->tevent_list_head);

    drv->remove = _def_trv_remove;

    return drv;
}


void    ats_tdrv_delete(ats_tdrv_t *tdrv)
{
    if (!tdrv)
    {
        return;
    }

    ats_tevent_t    *node = NULL;
    osa_list_t      *l = NULL;

    for (l = tdrv->tevent_list_head.next; l != &tdrv->tevent_list_head; l = l->next)
    {
        node = osa_list_entry(l, ats_tevent_t, list);
        if (node->ops.test_stop)
        {
            node->ops.test_stop(node);
        }
        if (node->ops.fini)
        {
            node->ops.fini(node);
        }
    }

    osa_list_remove(&tdrv->tevent_list_head);
    osa_mem_free(tdrv);
}


void ats_tdrv_do_test(ats_tdrv_t *drv)
{
    /** attention: char pointer */
    osa_uint8_t     *ptr = NULL;
    osa_uint32_t    i;

    osa_list_t      *l = NULL;
    ats_tevent_t    *node = NULL;

    for (l = drv->tevent_list_head.next; l != &drv->tevent_list_head; l = l->next)
    {
        node = osa_list_entry(l, ats_tevent_t, list);

        if (!node->ops.init)
            node->ops.init = _def_init;
        if (!node->ops.test_start)
            node->ops.test_start = _def_start_test;
        if (!node->ops.test_stop)
            node->ops.test_stop = _def_stop_test;
        if (!node->ops.suss_cb)
            node->ops.suss_cb = _def_suss_cb;
        if (!node->ops.fail_cb)
            node->ops.fail_cb = _def_fail_cb;
        if (!node->ops.fini)
            node->ops.fini = _def_fini;

        if (node->ops.init(node) != OSA_ERR_OK)
        {
            ats_log_error("Failed to begin test!\n");
            continue;
        }

        node->attr.stat.fail_times = 0;

        for (i = 0; i < node->attr.tcb.tc_num; i++)
        {
            ats_log_info("Test case : %d\n", i + 1);

            // 获取每一组测试用例数据的首地址
            ptr = node->attr.tcb.tc_cases + i * node->attr.tcb.tc_size;

            node->attr.result = node->ops.test_start((void *)ptr);

            if (node->attr.result == ATS_SUCCEED)
            {
                node->ops.suss_cb(node);
            }
            else if (node->attr.result == ATS_FAILED)
            {
                node->ops.fail_cb(node);
                node->attr.stat.fail_times ++;
            }
        }

        node->attr.stat.test_times = node->attr.tcb.tc_num;

        node->ops.fini(node);
    }
}


static osa_err_t _def_init(ats_tevent_t *tevent)
{
    return OSA_ERR_OK;
}

static ats_result_t _def_start_test(void *testCase)
{
    return ATS_SUCCEED;
}

static void _def_stop_test(ats_tevent_t *tevent)
{
}

static void _def_suss_cb(ats_tevent_t *tevent)
{
}

static void _def_fail_cb(ats_tevent_t *tevent)
{
}

static void _def_fini(ats_tevent_t *tevent)
{
#if 0
    char buf[1024] = {0};

    int testTimes = tevent->attr.stat.test_times;
    int failTimes = tevent->attr.stat.fail_times;
    sprintf(buf, "[%s] Test times : %d, Failed times : %d, Fail rate : %.2f%%", 
            tevent->name, testTimes, failTimes, failTimes / (testTimes * 1.0) * 100);

    ats_report_write(tevent->report, buf, strlen(buf));
#endif
}

static void _def_trv_remove(ats_tdrv_t *tevent)
{
    if (tevent)
    {
        ats_tdrv_delete(tevent);
    }
}
