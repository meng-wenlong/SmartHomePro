import React from 'react';
import 'antd/dist/antd.css';
import { Icon, Divider, Tooltip } from 'antd';
import { connect } from 'dva';

//连接绑定model的state到组件的props
const mapStateToProps = (state) => {
    const cardContent = state.main.alarmCard;
    return {
        cardContent,
    };
};
const mapDispatchToProps = (dispatch) => {
    return {
        clearAlarm() {
            dispatch({
                type: `main/clearAlarm`,
            });
        }
    };
};

@connect(mapStateToProps, mapDispatchToProps)
export default class AlarmStatusCard extends React.Component {

    render() {
        //清除警报
        const clearAlarm = () => this.props.clearAlarm();
        return (
            <div style={{ display: 'flex', alignItems: 'center', paddingTop: '5%', justifyContent: 'center', height: '100%', width: '100%' }}>
                <div style={{
                    position: 'relative',
                    border: '1px solid #e8e8e8',
                    borderRadius: '4px',
                    height: "100%", width: "100%",
                }}>
                    <div style={{
                        position: 'absolute', top: -20, left: 40,
                        height: '100px', width: '100px',
                        backgroundColor: '#e25858',
                        borderRadius: '4px',
                        boxShadow: '0 0 10px grey',
                        display: 'flex', alignItems: 'center', justifyContent: 'center',
                    }}
                    >
                        {
                            //从这里看出，判断温度阈值是否超标应该是后端干的事情(?)
                            this.props.cardContent.state === true ? (
                                //发生报警
                                <Tooltip title="点击按钮即可清除警报">
                                    <Icon
                                        //将点击行为绑定上取消alarm的函数
                                        onClick={clearAlarm}
                                        spin={true}
                                        type={"exclamation-circle"}
                                        style={{ color: 'white', fontSize: '60px' }}
                                    />
                                </Tooltip>
                            )
                                : (
                                    <Icon
                                        type={"check-circle"}
                                        style={{ color: 'white', fontSize: '60px' }}
                                    />
                                )
                        }
                    </div>
                    <div style={{ textAlign: 'right', paddingRight: '50px' }}>
                        <div style={{ fontSize: '16px', marginTop: '0px' }}>设备报警状态</div>
                        &nbsp;
                        &nbsp;
                        <div style={{ fontSize: '30px', fontWeight: 'bold' }}>{this.props.cardContent.state === true ? "报警" : "正常"}</div>
                    </div>
                    &nbsp;
                    &nbsp;
                    <Divider
                        style={{
                            marginTop: 0,
                            marginBottom: 0,
                            fontSize: '12px',
                            padding: '10px 5px 0 5px'
                        }}
                    >最近报警时间</Divider>
                    <div style={{ fontSize: '12px' }}>{this.props.cardContent.recentAlarmTime ? this.props.cardContent.recentAlarmTime : "暂未报警"}</div>
                </div>
            </div>
        );
    }
}
