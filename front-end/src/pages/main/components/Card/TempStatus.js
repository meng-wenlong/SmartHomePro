import React from 'react'
import 'antd/dist/antd.css';
import { Icon, Divider } from 'antd';
import { connect } from 'dva';

const mapStateToProps = (state) => {
    const cardContent = state.main.tempCard;
    return {
        cardContent,
    };
};

@connect(mapStateToProps)
export default class TempStatusCard extends React.Component {
    render() {
        return (
            // 这是设备实时温度外面的框框
            <div style={{ display: 'flex', alignItems: 'center', paddingTop: '5%', justifyContent: 'center', height: '100%', width: '100%' }}>
                <div style={{
                    position: 'relative',
                    border: '1px solid #e8e8e8',
                    height: "100%", width: "100%",
                    borderRadius: '4px',
                }}>
                    <div style={{
                        position: 'absolute', top: -20, left: 40,
                        height: '100px', width: '100px', backgroundColor: '#0099ff',
                        borderRadius: '4px', display: 'flex', alignItems: 'center',
                        justifyContent: 'center', boxShadow: '0 0 10px grey',
                    }}
                    >
                        <Icon
                            type="line-chart"
                            style={{ color: 'white', fontSize: '60px' }}
                        />
                    </div>
                    <div style={{ textAlign: 'right', paddingRight: '50px' }}>
                        <div style={{ ontSize: '16px', marginTop: '0px' }}>设备实时温度</div>
                        &nbsp;
                        &nbsp;
                        <div style={{ fontSize: '30px', fontWeight: 'bold' }}>{this.props.cardContent.value}℃</div>
                    </div>
                    &nbsp;
                    &nbsp;
                    <Divider style={{ marginTop: 0, marginBottom: 0, fontSize: '12px', padding: '10px 5px 0 5px' }}>温度报警阈值</Divider>
                    <div style={{ fontSize: '12px' }}> {this.props.cardContent.threshold}℃</div>
                </div>
            </div>
        );
    }
}
