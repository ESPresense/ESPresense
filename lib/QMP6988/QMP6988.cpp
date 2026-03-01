#include <math.h>
#include "stdint.h"
#include "stdio.h"
#include "QMP6988.h"

// DISABLE LOG
#define QMP6988_LOG(format...)
#define QMP6988_ERR(format...)

// ENABLE LOG
// #define QMP6988_LOG Serial.printf
// #define QMP6988_ERR Serial.printf

int QMP6988::getCalibrationData() {
    int status = 0;
    // BITFIELDS temp_COE;
    uint8_t a_data_uint8_tr[QMP6988_CALIBRATION_DATA_LENGTH] = {0};
    int len;

    for (len = 0; len < QMP6988_CALIBRATION_DATA_LENGTH; len += 1) {
        status = _i2c.readBytes(_addr, QMP6988_CALIBRATION_DATA_START + len,
                                &a_data_uint8_tr[len], 1);
        if (status == 0) {
            QMP6988_LOG("qmp6988 read 0xA0 error!");
            return status;
        }
    }

    qmp6988.qmp6988_cali.COE_a0 =
        (QMP6988_S32_t)(((a_data_uint8_tr[18] << SHIFT_LEFT_12_POSITION) |
                         (a_data_uint8_tr[19] << SHIFT_LEFT_4_POSITION) |
                         (a_data_uint8_tr[24] & 0x0f))
                        << 12);
    qmp6988.qmp6988_cali.COE_a0 = qmp6988.qmp6988_cali.COE_a0 >> 12;

    qmp6988.qmp6988_cali.COE_a1 =
        (QMP6988_S16_t)(((a_data_uint8_tr[20]) << SHIFT_LEFT_8_POSITION) |
                        a_data_uint8_tr[21]);
    qmp6988.qmp6988_cali.COE_a2 =
        (QMP6988_S16_t)(((a_data_uint8_tr[22]) << SHIFT_LEFT_8_POSITION) |
                        a_data_uint8_tr[23]);

    qmp6988.qmp6988_cali.COE_b00 =
        (QMP6988_S32_t)(((a_data_uint8_tr[0] << SHIFT_LEFT_12_POSITION) |
                         (a_data_uint8_tr[1] << SHIFT_LEFT_4_POSITION) |
                         ((a_data_uint8_tr[24] & 0xf0) >>
                          SHIFT_RIGHT_4_POSITION))
                        << 12);
    qmp6988.qmp6988_cali.COE_b00 = qmp6988.qmp6988_cali.COE_b00 >> 12;

    qmp6988.qmp6988_cali.COE_bt1 =
        (QMP6988_S16_t)(((a_data_uint8_tr[2]) << SHIFT_LEFT_8_POSITION) |
                        a_data_uint8_tr[3]);
    qmp6988.qmp6988_cali.COE_bt2 =
        (QMP6988_S16_t)(((a_data_uint8_tr[4]) << SHIFT_LEFT_8_POSITION) |
                        a_data_uint8_tr[5]);
    qmp6988.qmp6988_cali.COE_bp1 =
        (QMP6988_S16_t)(((a_data_uint8_tr[6]) << SHIFT_LEFT_8_POSITION) |
                        a_data_uint8_tr[7]);
    qmp6988.qmp6988_cali.COE_b11 =
        (QMP6988_S16_t)(((a_data_uint8_tr[8]) << SHIFT_LEFT_8_POSITION) |
                        a_data_uint8_tr[9]);
    qmp6988.qmp6988_cali.COE_bp2 =
        (QMP6988_S16_t)(((a_data_uint8_tr[10]) << SHIFT_LEFT_8_POSITION) |
                        a_data_uint8_tr[11]);
    qmp6988.qmp6988_cali.COE_b12 =
        (QMP6988_S16_t)(((a_data_uint8_tr[12]) << SHIFT_LEFT_8_POSITION) |
                        a_data_uint8_tr[13]);
    qmp6988.qmp6988_cali.COE_b21 =
        (QMP6988_S16_t)(((a_data_uint8_tr[14]) << SHIFT_LEFT_8_POSITION) |
                        a_data_uint8_tr[15]);
    qmp6988.qmp6988_cali.COE_bp3 =
        (QMP6988_S16_t)(((a_data_uint8_tr[16]) << SHIFT_LEFT_8_POSITION) |
                        a_data_uint8_tr[17]);

    QMP6988_LOG("<-----------calibration data-------------->\r\n");
    QMP6988_LOG("COE_a0[%d]	COE_a1[%d]	COE_a2[%d]	COE_b00[%d]\r\n",
                qmp6988.qmp6988_cali.COE_a0, qmp6988.qmp6988_cali.COE_a1,
                qmp6988.qmp6988_cali.COE_a2, qmp6988.qmp6988_cali.COE_b00);
    QMP6988_LOG("COE_bt1[%d]	COE_bt2[%d]	COE_bp1[%d]	COE_b11[%d]\r\n",
                qmp6988.qmp6988_cali.COE_bt1, qmp6988.qmp6988_cali.COE_bt2,
                qmp6988.qmp6988_cali.COE_bp1, qmp6988.qmp6988_cali.COE_b11);
    QMP6988_LOG("COE_bp2[%d]	COE_b12[%d]	COE_b21[%d]	COE_bp3[%d]\r\n",
                qmp6988.qmp6988_cali.COE_bp2, qmp6988.qmp6988_cali.COE_b12,
                qmp6988.qmp6988_cali.COE_b21, qmp6988.qmp6988_cali.COE_bp3);
    QMP6988_LOG("<-----------calibration data-------------->\r\n");

    qmp6988.ik.a0  = qmp6988.qmp6988_cali.COE_a0;   // 20Q4
    qmp6988.ik.b00 = qmp6988.qmp6988_cali.COE_b00;  // 20Q4

    qmp6988.ik.a1 = 3608L * (QMP6988_S32_t)qmp6988.qmp6988_cali.COE_a1 -
                    1731677965L;  // 31Q23
    qmp6988.ik.a2 = 16889L * (QMP6988_S32_t)qmp6988.qmp6988_cali.COE_a2 -
                    87619360L;  // 30Q47

    qmp6988.ik.bt1 = 2982L * (QMP6988_S64_t)qmp6988.qmp6988_cali.COE_bt1 +
                     107370906L;  // 28Q15
    qmp6988.ik.bt2 = 329854L * (QMP6988_S64_t)qmp6988.qmp6988_cali.COE_bt2 +
                     108083093L;  // 34Q38
    qmp6988.ik.bp1 = 19923L * (QMP6988_S64_t)qmp6988.qmp6988_cali.COE_bp1 +
                     1133836764L;  // 31Q20
    qmp6988.ik.b11 = 2406L * (QMP6988_S64_t)qmp6988.qmp6988_cali.COE_b11 +
                     118215883L;  // 28Q34
    qmp6988.ik.bp2 = 3079L * (QMP6988_S64_t)qmp6988.qmp6988_cali.COE_bp2 -
                     181579595L;  // 29Q43
    qmp6988.ik.b12 = 6846L * (QMP6988_S64_t)qmp6988.qmp6988_cali.COE_b12 +
                     85590281L;  // 29Q53
    qmp6988.ik.b21 = 13836L * (QMP6988_S64_t)qmp6988.qmp6988_cali.COE_b21 +
                     79333336L;  // 29Q60
    qmp6988.ik.bp3 = 2915L * (QMP6988_S64_t)qmp6988.qmp6988_cali.COE_bp3 +
                     157155561L;  // 28Q65
    QMP6988_LOG("<----------- int calibration data -------------->\r\n");
    QMP6988_LOG("a0[%d]	a1[%d] a2[%d] b00[%d]\r\n", qmp6988.ik.a0,
                qmp6988.ik.a1, qmp6988.ik.a2, qmp6988.ik.b00);
    QMP6988_LOG("bt1[%lld]	bt2[%lld]	bp1[%lld]	b11[%lld]\r\n",
                qmp6988.ik.bt1, qmp6988.ik.bt2, qmp6988.ik.bp1, qmp6988.ik.b11);
    QMP6988_LOG("bp2[%lld]	b12[%lld]	b21[%lld]	bp3[%lld]\r\n",
                qmp6988.ik.bp2, qmp6988.ik.b12, qmp6988.ik.b21, qmp6988.ik.bp3);
    QMP6988_LOG("<----------- int calibration data -------------->\r\n");
    return 1;
}

QMP6988_S16_t QMP6988::convTx02e(qmp6988_ik_data_t* ik, QMP6988_S32_t dt) {
    QMP6988_S16_t ret;
    QMP6988_S64_t wk1, wk2;

    // wk1: 60Q4 // bit size
    wk1 = ((QMP6988_S64_t)ik->a1 * (QMP6988_S64_t)dt);  // 31Q23+24-1=54 (54Q23)
    wk2 = ((QMP6988_S64_t)ik->a2 * (QMP6988_S64_t)dt) >>
          14;                                    // 30Q47+24-1=53 (39Q33)
    wk2 = (wk2 * (QMP6988_S64_t)dt) >> 10;       // 39Q33+24-1=62 (52Q23)
    wk2 = ((wk1 + wk2) / 32767) >> 19;           // 54,52->55Q23 (20Q04)
    ret = (QMP6988_S16_t)((ik->a0 + wk2) >> 4);  // 21Q4 -> 17Q0
    return ret;
}

QMP6988_S32_t QMP6988::getPressure02e(qmp6988_ik_data_t* ik, QMP6988_S32_t dp,
                                      QMP6988_S16_t tx) {
    QMP6988_S32_t ret;
    QMP6988_S64_t wk1, wk2, wk3;

    // wk1 = 48Q16 // bit size
    wk1 =
        ((QMP6988_S64_t)ik->bt1 * (QMP6988_S64_t)tx);  // 28Q15+16-1=43 (43Q15)
    wk2 = ((QMP6988_S64_t)ik->bp1 * (QMP6988_S64_t)dp) >>
          5;     // 31Q20+24-1=54 (49Q15)
    wk1 += wk2;  // 43,49->50Q15
    wk2 = ((QMP6988_S64_t)ik->bt2 * (QMP6988_S64_t)tx) >>
          1;                               // 34Q38+16-1=49 (48Q37)
    wk2 = (wk2 * (QMP6988_S64_t)tx) >> 8;  // 48Q37+16-1=63 (55Q29)
    wk3 = wk2;                             // 55Q29
    wk2 = ((QMP6988_S64_t)ik->b11 * (QMP6988_S64_t)tx) >>
          4;                               // 28Q34+16-1=43 (39Q30)
    wk2 = (wk2 * (QMP6988_S64_t)dp) >> 1;  // 39Q30+24-1=62 (61Q29)
    wk3 += wk2;                            // 55,61->62Q29
    wk2 = ((QMP6988_S64_t)ik->bp2 * (QMP6988_S64_t)dp) >>
          13;                              // 29Q43+24-1=52 (39Q30)
    wk2 = (wk2 * (QMP6988_S64_t)dp) >> 1;  // 39Q30+24-1=62 (61Q29)
    wk3 += wk2;                            // 62,61->63Q29
    wk1 += wk3 >> 14;                      // Q29 >> 14 -> Q15
    wk2 =
        ((QMP6988_S64_t)ik->b12 * (QMP6988_S64_t)tx);  // 29Q53+16-1=45 (45Q53)
    wk2 = (wk2 * (QMP6988_S64_t)tx) >> 22;             // 45Q53+16-1=61 (39Q31)
    wk2 = (wk2 * (QMP6988_S64_t)dp) >> 1;              // 39Q31+24-1=62 (61Q30)
    wk3 = wk2;                                         // 61Q30
    wk2 = ((QMP6988_S64_t)ik->b21 * (QMP6988_S64_t)tx) >>
          6;                                // 29Q60+16-1=45 (39Q54)
    wk2 = (wk2 * (QMP6988_S64_t)dp) >> 23;  // 39Q54+24-1=62 (39Q31)
    wk2 = (wk2 * (QMP6988_S64_t)dp) >> 1;   // 39Q31+24-1=62 (61Q20)
    wk3 += wk2;                             // 61,61->62Q30
    wk2 = ((QMP6988_S64_t)ik->bp3 * (QMP6988_S64_t)dp) >>
          12;                               // 28Q65+24-1=51 (39Q53)
    wk2 = (wk2 * (QMP6988_S64_t)dp) >> 23;  // 39Q53+24-1=62 (39Q30)
    wk2 = (wk2 * (QMP6988_S64_t)dp);        // 39Q30+24-1=62 (62Q30)
    wk3 += wk2;                             // 62,62->63Q30
    wk1 += wk3 >> 15;                       // Q30 >> 15 = Q15
    wk1 /= 32767L;
    wk1 >>= 11;      // Q15 >> 7 = Q4
    wk1 += ik->b00;  // Q4 + 20Q4
    // wk1 >>= 4; // 28Q4 -> 24Q0
    ret = (QMP6988_S32_t)wk1;
    return ret;
}

void QMP6988::reset() {
    uint8_t ret = 0;

    ret = _i2c.writeByte(_addr, QMP6988_RESET_REG, 0xe6);
    if (ret == 0) {
        QMP6988_LOG("reset fail!!! \r\n");
    }
    delay(20);
    ret = _i2c.writeByte(_addr, QMP6988_RESET_REG, 0x00);
}

void QMP6988::setpPowermode(int power_mode) {
    uint8_t data;

    QMP6988_LOG("qmp_set_powermode %d \r\n", power_mode);

    qmp6988.power_mode = power_mode;
    _i2c.readBytes(_addr, QMP6988_CTRLMEAS_REG, &data, 1);
    data = data & 0xfc;
    if (power_mode == QMP6988_SLEEP_MODE) {
        data |= 0x00;
    } else if (power_mode == QMP6988_FORCED_MODE) {
        data |= 0x01;
    } else if (power_mode == QMP6988_NORMAL_MODE) {
        data |= 0x03;
    }
    _i2c.writeByte(_addr, QMP6988_CTRLMEAS_REG, data);

    QMP6988_LOG("qmp_set_powermode 0xf4=0x%x \r\n", data);

    delay(20);
}

void QMP6988::setFilter(unsigned char filter) {
    uint8_t data;

    data = (filter & 0x03);
    _i2c.writeByte(_addr, QMP6988_CONFIG_REG, data);

    delay(20);
}

void QMP6988::setOversamplingP(unsigned char oversampling_p) {
    uint8_t data;

    _i2c.readBytes(_addr, QMP6988_CTRLMEAS_REG, &data, 1);
    data &= 0xe3;
    data |= (oversampling_p << 2);
    _i2c.writeByte(_addr, QMP6988_CTRLMEAS_REG, data);
    delay(20);
}

void QMP6988::setOversamplingT(unsigned char oversampling_t) {
    uint8_t data;

    _i2c.readBytes(_addr, QMP6988_CTRLMEAS_REG, &data, 1);
    data &= 0x1f;
    data |= (oversampling_t << 5);
    _i2c.writeByte(_addr, QMP6988_CTRLMEAS_REG, data);
    delay(20);
}

float QMP6988::calcAltitude(float pressure, float temp) {
    float altitude;

    altitude =
        (pow((101325 / pressure), 1 / 5.257) - 1) * (temp + 273.15) / 0.0065;
    QMP6988_LOG("altitude = %f\r\n", altitude);
    return altitude;
}

float QMP6988::calcPressure() {
    uint8_t err = 0;
    QMP6988_U32_t P_read, T_read;
    QMP6988_S32_t P_raw, T_raw;
    uint8_t a_data_uint8_tr[6] = {0};
    QMP6988_S32_t T_int, P_int;

    // press
    err = _i2c.readBytes(_addr, QMP6988_PRESSURE_MSB_REG, a_data_uint8_tr, 6);
    if (err == 0) {
        QMP6988_LOG("qmp6988 read press raw error! \r\n");
        return 0.0f;
    }
    P_read = (QMP6988_U32_t)((((QMP6988_U32_t)(a_data_uint8_tr[0]))
                              << SHIFT_LEFT_16_POSITION) |
                             (((QMP6988_U16_t)(a_data_uint8_tr[1]))
                              << SHIFT_LEFT_8_POSITION) |
                             (a_data_uint8_tr[2]));
    P_raw  = (QMP6988_S32_t)(P_read - SUBTRACTOR);

    T_read = (QMP6988_U32_t)((((QMP6988_U32_t)(a_data_uint8_tr[3]))
                              << SHIFT_LEFT_16_POSITION) |
                             (((QMP6988_U16_t)(a_data_uint8_tr[4]))
                              << SHIFT_LEFT_8_POSITION) |
                             (a_data_uint8_tr[5]));
    T_raw  = (QMP6988_S32_t)(T_read - SUBTRACTOR);

    T_int               = convTx02e(&(qmp6988.ik), T_raw);
    P_int               = getPressure02e(&(qmp6988.ik), P_raw, T_int);
    qmp6988.temperature = (float)T_int / 256.0f;
    qmp6988.pressure    = (float)P_int / 16.0f;

    return qmp6988.pressure;
}

float QMP6988::calcTemperature() {
    uint8_t err = 0;
    QMP6988_U32_t P_read, T_read;
    QMP6988_S32_t P_raw, T_raw;
    uint8_t a_data_uint8_tr[6] = {0};
    QMP6988_S32_t T_int, P_int;

    // press
    err = _i2c.readBytes(_addr, QMP6988_PRESSURE_MSB_REG, a_data_uint8_tr, 6);
    if (err == 0) {
        QMP6988_LOG("qmp6988 read press raw error! \r\n");
        return 0.0f;
    }
    P_read = (QMP6988_U32_t)((((QMP6988_U32_t)(a_data_uint8_tr[0]))
                              << SHIFT_LEFT_16_POSITION) |
                             (((QMP6988_U16_t)(a_data_uint8_tr[1]))
                              << SHIFT_LEFT_8_POSITION) |
                             (a_data_uint8_tr[2]));
    P_raw  = (QMP6988_S32_t)(P_read - SUBTRACTOR);

    // temp
    err =
        _i2c.readBytes(_addr, QMP6988_TEMPERATURE_MSB_REG, a_data_uint8_tr, 3);
    if (err == 0) {
        QMP6988_LOG("qmp6988 read temp raw error! \n");
    }
    T_read = (QMP6988_U32_t)((((QMP6988_U32_t)(a_data_uint8_tr[3]))
                              << SHIFT_LEFT_16_POSITION) |
                             (((QMP6988_U16_t)(a_data_uint8_tr[4]))
                              << SHIFT_LEFT_8_POSITION) |
                             (a_data_uint8_tr[5]));
    T_raw  = (QMP6988_S32_t)(T_read - SUBTRACTOR);

    T_int               = convTx02e(&(qmp6988.ik), T_raw);
    P_int               = getPressure02e(&(qmp6988.ik), P_raw, T_int);
    qmp6988.temperature = (float)T_int / 256.0f;
    qmp6988.pressure    = (float)P_int / 16.0f;

    return qmp6988.temperature;
}

bool QMP6988::begin(TwoWire* wire, uint8_t addr, uint8_t sda, uint8_t scl,
                    long freq) {
    _i2c.begin(wire, sda, scl, freq);
    _addr = addr;
    if (!_i2c.exist(_addr)) {
        return false;
    }
    reset();
    getCalibrationData();
    setpPowermode(QMP6988_NORMAL_MODE);
    setFilter(QMP6988_FILTERCOEFF_4);
    setOversamplingP(QMP6988_OVERSAMPLING_8X);
    setOversamplingT(QMP6988_OVERSAMPLING_1X);
    return true;
}

bool QMP6988::update() {
    pressure = calcPressure();
    cTemp    = calcTemperature();
    altitude = calcAltitude(pressure, cTemp);
    return true;
}
