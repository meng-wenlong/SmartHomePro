import React from 'react'
import 'antd/dist/antd.css';
import { Icon, Divider } from 'antd';
import { connect } from 'dva';

//连接绑定model的state到组件的props
const mapStateToProps = (state) => {
    const cardContent = state.main.humiCard;
    return {
        cardContent,
    };
};

@connect(mapStateToProps)
export default class HumiStatusCard extends React.Component {

    render() {
        return (
            <div style={{ display: 'flex', alignItems: 'center', paddingTop: '5%', justifyContent: 'center', height: '100%', width: '100%' }}>
                <div style={{
                    position: 'relative',
                    border: '1px solid #e8e8e8', borderRadius: '4px',
                    height: "100%", width: "100%",
                }}>
                    <div style={{
                        position: 'absolute', top: -20, left: 40,
                        height: '100px', width: '100px',
                        backgroundColor: '#33cc33',
                        borderRadius: '4px', boxShadow: '0 0 10px grey',
                        display: 'flex', alignItems: 'center', justifyContent: 'center'
                    }}
                    >
                        <Icon type="dot-chart" style={{ color: 'white', fontSize: '60px' }} />
                    </div>
                    <div style={{ textAlign: 'right', paddingRight: '50px' }}>
                        <div style={{ fontSize: '16px', marginTop: '0px' }}>设备实时湿度</div>
                        &nbsp;
                        &nbsp;
                        <div style={{ fontSize: '30px', fontWeight: 'bold' }}>{this.props.cardContent.value}%</div>
                    </div>
                    &nbsp;
                    &nbsp;
                    <Divider style={{ marginTop: 0, marginBottom: 0, fontSize: '12px', padding: '10px 5px 0 5px' }}>湿度传感器</Divider>
                    <div style={{ fontSize: '12px' }}>DH11传感器</div>
                </div>
            </div>
        );
    }
}
