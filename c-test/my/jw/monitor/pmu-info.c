/*******************************************************************************
* Author : liyunteng
* Email : li_yunteng@163.com
* Created Time : 2014-02-14 11:38
* Filename : pmu-info.c
* Description : 
* *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <strings.h>
#include <string.h>
#include "sys-mon.h"
#include "pmu-info.h"

#define VOUT_EXP (-9)

#define CHECK_FAULT(sts, bit, set)	\
	if ((sts) & (bit))		\
		(set) = 1;		\
	else				\
		(set) = 0;

enum {

    REG_STATUS = 0x79,
    REG_VIN = 0x88,
    REG_VOUT = 0x8b,
    REG_TEMP = 0x8e,
    REG_FAN = 0x90,

    STS_TEMP_FAULT = (1 << 2),
    STS_VIN_FAULT = (1 << 3),
    STS_VOUT_FAULT = (1 << 5),
    STS_FAN_FAULT = (1 << 10)
};

static float pmu_linear_to_real(uint16_t v)
{
    int16_t exp, data;

    data = v & 0x7ff;
    data = (data << 5) >> 5;
    exp = v & 0xf800;
    exp = exp >> 11;

    return (float) data *pow(2, (float) exp);
}


extern int global_case_temp;
int pmu_get_info(const char *dev, struct pmu_info *info1, int check_temp)
{
    FILE *fp;
    char line[128];
    unsigned long sts, vin, vout, fan, temp_amb, temp_hs;

    fp = fopen(dev, "r");
    if (!fp) {
	return -1;
    }

    fread(line, sizeof(line), 1, fp);
    fclose(fp);

    sscanf(line, "%lx %lx %lx %lx %lx %lx",
	   &sts, &vin, &vout, &fan, &temp_amb, &temp_hs);
    bzero(info1, sizeof(struct pmu_info));
    CHECK_FAULT(sts, STS_TEMP_FAULT, info1->is_temp_fault);
    CHECK_FAULT(sts, STS_VIN_FAULT, info1->is_vin_fault);
    CHECK_FAULT(sts, STS_VOUT_FAULT, info1->is_vout_fault);
    CHECK_FAULT(sts, STS_FAN_FAULT, info1->is_fan_fault);
    info1->vin = pmu_linear_to_real(vin);
    info1->vout = vout * pow(2, VOUT_EXP);
    info1->fan_speed = pmu_linear_to_real(fan);
    info1->temp = pmu_linear_to_real(temp_amb);

    if (info1->vin < 100.0)
	info1->is_vin_fault = 1;

    if (info1->fan_speed > 4000.0)
	info1->is_fan_fault = 0;

#ifdef _DEBUG
    printf("power-module%c, sts: 0x%x, vin: %.1f(0x%x), vout: %.1f(0x%x),"
	   "fan_speed: %.1f(0x%x), temp_amb: %.1f(0x%x), temp_hs: %.1f(0x%x)",
	   dev[strlen(dev) - 1], sts, info1->vin, vin, info1->vout, vout,
	   info1->fan_speed, fan, info1->temp, temp_amb,
	   pmu_linear_to_real(temp_hs), temp_hs);
#endif


#define POWER_TEMP_HI 50
#define POWER_TEMP_MI 46
#define POWER_TEMP_LO 40
#define POWER_FAN_LO 8000

    char buf[64] = { '\0' };
    int power_temp_mi = POWER_TEMP_MI;
    int power_temp_lo = POWER_TEMP_LO;

    if (!check_temp)
	return 0;

    if (global_case_temp > POWER_TEMP_MI) {
	power_temp_mi = global_case_temp;
	power_temp_lo = global_case_temp - 2;
    }

    if (info1->temp > 100) {
	return 0;
    } else if (info1->temp > POWER_TEMP_HI) {
	strcpy(buf, "fan_speed set 100");
    } else if (info1->temp < POWER_TEMP_LO
	       && info1->fan_speed < POWER_FAN_LO) {
	return 0;
    } else if (info1->temp <= power_temp_lo) {
	strcpy(buf, "fan_speed dec 10");
    } else if (info1->temp > power_temp_mi) {
	strcpy(buf, "fan_speed inc 10");
    } else {
	return 0;
    }

    fp = fopen(dev, "w");
    if (!fp) {
	return -1;
    }

    fwrite(buf, strlen(buf), 1, fp);
    fclose(fp);

    return 0;


}
