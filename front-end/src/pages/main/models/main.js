import {
    getDeviceList,
    queryDeviceProp,
    selectChartDataByTime,
    setDeviceProperty,
    setDeviceLightProperty,
    queryAlarmHistoryLogs,
    clearAlarm
} from '../../../axios/index';
import { message } from 'antd';


export default {
    //储存model的state状态
    state: {
        deviceName: "",
        deviceSelectList: [],
        alarmCard: {
            state: false,
            recentAlarmTime: null
        },
        onlineCard: {
            state: "离线",
            recentOnlineTime: null
        },
        tempCard: {
            value: 22,
            threshold: 24
        },
        // 修改
        lightCard: {
            value: 2,
            threshold: 15
        },
        // 修改结束
        humiCard: {
            value: 43,
            sensor: "DHT11温湿度传感器"
        },
        updateTime: "待初始化",
        chart: {
            dataList: [],
        },
        table: {
            data: []
        }
    },

    reducers: {
        update(state, { payload }) {
            let newState = payload;
            return {
                ...state,
                ...newState
            };
        },
    },
    //异步操作，通过reducers间接更新state，返回的结果先调用reducer
    effects: {
        *getDeviceList({ payload }, { call, put }) {
            let res = yield call(getDeviceList);
            yield put({
                type: 'update',
                payload: {
                    deviceName: res.data[0] !== null ? res.data[0]:"暂无设备",
                    deviceSelectList: res.data
                },
            });
        },
        //周期性更新数据，封装成newState更新
        *queryDeviceProp({ payload }, { call, put }) {
            let res = yield call(queryDeviceProp, payload);
            yield put({
                type: 'update',
                payload: {
                    alarmCard: {
                        state: res.data.alarmStatus,
                        recentAlarmTime: res.data.lastAlarmDate
                    },
                    onlineCard: {
                        state: res.data.connectStatus,
                        recentOnlineTime: res.data.lastOnlineDate
                    },
                    tempCard: {
                        value: res.data.currentTemperature,
                        threshold: res.data.tempThreshold
                    },
                    //修改(需要与后端核对“currentLightIntensity“的名字)
                    lightCard: {
                        value: res.data.currentLight,
                        threshold: res.data.lightThreshold
                        //threshold: 22
                    },
                    //修改结束
                    humiCard: {
                        value: res.data.currentHumidity,
                        sensor: "DHT11温湿度传感器"
                    },
                    updateTime: res.data.gmtUpdate
                },
            });
        },
        //根据起止时间查询设备温湿度数据
        *selectChartDataByTime({ payload }, { call, put, select }) {
            let deviceName = yield select(state => state.main.deviceName);
            let res = yield call(selectChartDataByTime, { ...payload, deviceName: deviceName });
            yield put({
                type: 'update',
                payload: {
                    chart: {
                        dataList: res.data
                    },
                }
            });
        },
        //设定报警超限值
        * setAlarmThreshold({ payload }, { call, select }) {
            let deviceName = yield select(state => state.main.deviceName);
            let res = yield call(setDeviceProperty, { ...payload, deviceName: deviceName });
            if (res.data.isSuccess === true) {
                message.success('提交成功···');
            }
        },

        // 修改
        //设定光照强度报警超限值
        * setLightAlarmThreshold({ payload }, { call, select }) {
            let deviceName = yield select(state => state.main.deviceName);
            let res = yield call(setDeviceLightProperty, { ...payload, deviceName: deviceName });
            if (res.data.isSuccess === true) {
                message.success('提交成功···');
            }
        },
        // 修改结束

        //根据起止时间查询报警数据
        * selectTableDataByTime({ payload }, { call, put, select }) {
            let deviceName = yield select(state => state.main.deviceName);
            let res = yield call(queryAlarmHistoryLogs, { ...payload, deviceName: deviceName });
            yield put({
                type: 'update',
                payload: {
                    table: {
                        data: res.data
                    }
                }
            });
        },

        //清除报警
        * clearAlarm({ payload }, { call, select }) {
            console.log("执行了");
            let deviceName = yield select(state => state.main.deviceName);
            let res = yield call(clearAlarm, { deviceName: deviceName });
            if (res.code === 0) {
                message.success('提交成功···');
            }
        },
    }
}