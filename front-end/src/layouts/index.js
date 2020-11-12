import React from 'react';
import 'antd/dist/antd.css';
import { Layout, Select } from 'antd';
import { connect } from 'dva';
import router from 'umi/router';

const { Header, Footer, Content } = Layout;
const Option = Select.Option;
//连接绑定model的state到组件的props
const mapStateToProps = (state) => {
  const headerContent = state.main.deviceSelectList;
  const deviceName = state.main.deviceName;
  return {
    headerContent,
    deviceName
  };
};
const mapDispatchToProps = (dispatch) => {
  return {
    onWillMount() {
      dispatch({
        type: `main/getDeviceList`,
      });
    },
    peroidlyUpdate(params) {
      dispatch({
        type: `main/queryDeviceProp`,
        payload: params
      })
    },
    onDeviceSelected(params) {
      dispatch({
        type: 'main/update',
        payload: params
      })
    }
  };
};

@connect(mapStateToProps, mapDispatchToProps)
export default class BasicLayout extends React.Component {

  constructor(props) {
    super(props);
    this.state = {
      inteval: setInterval(async () => {
        await this.props.peroidlyUpdate({
          deviceName: this.props.deviceName
        });
        console.log("inteval is working, querying device is " + this.props.deviceName);
      }, 2000)
    }
  }

  async componentWillMount() {
    await this.props.onWillMount();
  }

  componentWillUnmount(){
    clearInterval(this.state.inteval);
  }

  render() {
    const onSelect = (value) => {
      this.props.onDeviceSelected({ deviceName: value});
      clearInterval(this.state.inteval);
      console.log("clear inteval");
      this.setState({
        inteval: setInterval(async () => {
          await this.props.peroidlyUpdate({
            deviceName: this.props.deviceName
          });
          console.log("inteval is working, querying device is " + this.props.deviceName);
        }, 2000)
      });
      console.log("set inteval");
    }
    return (
      <div>
        <Layout>
          <Header style={{ display: 'flex', justifyContent: 'space-between' }}>
            <div style={{ color: "white", fontSize: '24px', fontWeight: 'bold', float: 'left' }}>阿里云IoT | ST 智能家居教程</div>
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', color: 'white' }}>请选择设备deviceName:
              &nbsp;
              <Select style={{ width: '200px', float: 'right' }}
                placeholder="请选择加载设备"
                onChange={onSelect}
              >
                {
                  this.props.headerContent.map(element => {
                    return <Option value={element}>{element}</Option>
                  })
                }
              </Select>
            </div>
          </Header>
          <Content>
            {this.props.children}
          </Content>
          <Footer style={{ textAlign: 'center', backgroundColor: '#001529' }}>
            <div style={{ color: "white" }}>
              天尊杨戢队
          </div>
          </Footer>
        </Layout>
      </div>
    );
  }
}
