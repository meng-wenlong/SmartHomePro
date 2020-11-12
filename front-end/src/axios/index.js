import http from './tools';
import {
  message
} from 'antd';
//const url = 'http://47.110.237.40:9099/api/v1/device';
// const url = 'http://localhost:9099/api/v1/device';
 const url = 'http://10.19.75.185:9099/api/v1/device';

const checkStatus = (res) => {
  if (res.status !== 200) {
    message.error('请求失败或网络异常，请检查···');
  } else {
    if (res.data.code !== 0) {
      message.error('提交失败或服务器异常，请检查···');
    }
  }
}

//查询设备的列表
export const getDeviceList = async () => {
  //得到原始输出
  let res = await http.get(url + '/listDeviceName', null);
  checkStatus(res);
  return res.data;
};

//查询设备的属性
export const queryDeviceProp = async (params) => {
  let res = await http.get(url + '/queryDeviceProp', params);
  checkStatus(res);
  return res.data;
}

//查询设备的属性
export const selectChartDataByTime = async (params) => {
  let res = await http.get(url + '/queryDevicePropHistoryLogs', params);
  checkStatus(res);
  return res.data;
}

//设置报警阈值
export const setDeviceProperty = async (params) => {
  let res = await http.get(url + '/setDeviceProperty', params);
  checkStatus(res);
  return res.data
}

//修改
//设置光照强度报警阈值
export const setDeviceLightProperty = async (params) => {
  let res = await http.get(url + '/setDeviceLightProperty', params);
  checkStatus(res);
  return res.data
}
//修改结束

//查询历史报警记录
export const queryAlarmHistoryLogs = async (params) => {
  let res = await http.get(url + '/queryAlarmHistoryLogs', params);
  checkStatus(res);
  return res.data
}

//清除报警效果
export const clearAlarm = async (params) => {
  let res = await http.get(url + '/clearAlarm', params);
  checkStatus(res);
  return res.data
}
