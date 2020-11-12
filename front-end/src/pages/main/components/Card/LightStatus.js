import React from 'react'
import 'antd/dist/antd.css';
import { Icon, Divider } from 'antd';
import { connect } from 'dva';

const mapStateToProps = (state) => {
    const cardContent = state.main.lightCard;
    return {
        cardContent,
    };
};

@connect(mapStateToProps)
export default class LightStatusCard extends React.Component {
    render() {
        return (
            <div style={{ display: 'flex', alignItems: 'center', paddingTop: '5%', justifyContent: 'center', height: '100%', width: '100%' }}>
                <div style={{
                    position: 'relative',
                    border: '1px solid #e8e8e8',
                    height: "100%", width: "100%",
                    borderRadius: '4px',
                }}>
                    <div style={{
                        position: 'absolute', top: -20, left: 40,
                        height: '100px', width: '100px', backgroundColor: '#cc3380',
                        borderRadius: '4px', display: 'flex', alignItems: 'center',
                        justifyContent: 'center', boxShadow: '0 0 10px grey',
                    }}
                    >
                        <Icon
                            type="area-chart"
                            style={{ color: 'white', fontSize: '60px' }}
                        />
                    </div>
                    <div style={{ textAlign: 'right', paddingRight: '30px' }}>
                        <div style={{ ontSize: '18px', marginTop: '0px' }}>设备实时光照强度</div>
                        &nbsp;
                        &nbsp;
                        <div style={{ fontSize: '30px', fontWeight: 'bold' }}>{this.props.cardContent.value}lx</div>
                    </div>
                    &nbsp;
                    &nbsp;
                    <Divider style={{ marginTop: 0, marginBottom: 0, fontSize: '12px', padding: '10px 5px 0 5px' }}>光照强度报警阈值</Divider>
                    <div style={{ fontSize: '12px' }}> {this.props.cardContent.threshold}lx</div>
                </div>
            </div>
        );
    }
}
