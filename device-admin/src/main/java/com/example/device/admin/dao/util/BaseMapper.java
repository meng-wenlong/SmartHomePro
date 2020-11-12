package com.example.device.admin.dao.util;

import tk.mybatis.mapper.common.Mapper;
import tk.mybatis.mapper.common.MySqlMapper;

/**
 * BaseMapper:特别注意，该接口不能被扫描到，否则会出错
 * 用于对MySQL操作
 * @author hj
 * @since 2018-12-16 21:53
 */
public interface BaseMapper<T> extends Mapper<T>, MySqlMapper<T> {

}