import axios from 'axios'
import qs from 'qs';
//通过async和await进行promise对象的处理

export default {
  async post(url, data) {
    return await axios({
      method: 'post',
      url,
      data: qs.stringify(data),   //post请求参数
      timeout: 10000, //超时时间
    })
  },
  async get(url, data) {
    return await axios({
      method: 'get',
      url,
      params: data, //get请求时带的参数
      timeout: 10000, //超时时间
    })
  }
}
