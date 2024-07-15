const app =getApp() /* 实例化*/
/* 页面参数 */
Page({
  /* 数据参数（自定义，在index.wxml调用参数） */
  data: {
    head_title:"首页",  /* 页面标题 */
    chiptemperature_title:"芯片温度", /* 芯片温度标题 */
    chiptemperature_value:"0", /* 芯片温度变量 */
    temperature_value:"0", /* 温度变量 */
    humidity_value:"0",  /* 湿度变量 */
    isConnect:false,  /* 服务器是否连接检测标识 */
    show:false, /* 弹窗开关检测标识 */

    /* LED灯参数 */
    Ledname:"LED灯",     /* LED名字 */
    LedisOpen:false,        /* 开关状态 */
    Ledslider_value:"0", /* 滑动状态 */


    /* 定时器参数初始化 */
    Numbers: 0,
    NumbersTimeOut: 0,
    Interval: null,
    timeOut: null,

    /* 传感器设备内容数组 */
    sensorslist:[
      {
        img:"../../images/thermometer (1).png", /* 图标 */
        name:"DHT11",                           /* 名字 */
        parameter:"温度",                       /* 内容 */
        value:0,                                /* 变量 */
        unit:"℃",                              /* 单位 */
        idx:0,                                  /* key序号 */
      },
      {
        img:"../../images/humidity.png",        /* 图标 */
        name:"DHT11",                           /* 名字 */
        parameter:"湿度",                       /* 内容 */
        value:0,                                /* 变量 */
        unit:"%RH",                             /* 单位 */
        idx:1,                                  /* key序号 */
      }
    ],
  },

  /* 按键开关灯事件函数 */
  onChange:function(event){
    let value = event.detail.value  /* 获取按键当前状态 */
    console.log("灯的状态目前为",value) /* 调试窗口显示value内容 */
    /* 判断按钮的Bool值，true or false */
    if(value == 1)
    {
      this.openLed()  /* 执行开灯函数 */
    }
    else
    {
      this.closeLed() /* 执行关灯函数 */
    } 
  },

  /* 打开Info弹窗函数 */ 
  showPopup:function() {
    this.setData({ show: true }); /* 调用setData去修改页面参数变量 */
  },

 /* 关闭Info弹窗函数 */ 
  onClose() {
    this.setData({ show: false });  /* 调用setData去修改页面参数变量 */
  },


  /* 页面初加载函数 */
  onLoad(){
    setInterval(this.CheckOnenet, 100);  /* 设置定时器(调用CheckOnenet函数，100ms) */
    setInterval(this.updatevalue, 100);  /* 设置定时器(调用updatevalue函数，100ms) */
  },

 /* 检测设备是否连接函数 */ 
  CheckOnenet:function(){
   var that = this;
  /* 发送request请求 */ 
  wx.request({
     /* 官方调试API地址(具体看文档说明) */  
    url: 'https://iot-api.heclouds.com/device/detail',
     /* request的操作方式(POST或者GET) */  
    method: 'GET',
    header: {
      /* 用户鉴权信息(使用token2.0生成，具体如word文档内说明) */  
      "authorization": "version=2022-05-01&res=userid%2F395514&et=1723879594&method=sha1&sign=r9OIGdid3W0EWWGgkfR7I%2BzQ15Y%3D"
    },
    /* 发送的body信息 */  
    data: {
      product_id:'4iXR2ZI4Tc',/* 产品ID */  
      device_name:'TemNHum',/* 设备名称 */  
    },
    /* request执行成功后返回数据存到res */ 
    success: res =>{
      if(res.data.data.status == 1) /* 判断设备是否在线(0:不在线 1:在线) */
      {
         /* 调用setData去修改页面参数变量 */
        that.setData({
          isConnect:true, /* isConnect修改为false */
        })
      }
      if(res.data.data.status == 0)
      {
         /* 调用setData去修改页面参数变量 */
        that.setData({
          isConnect:false, /* isConnect修改为true */
        })
      }
    },
    /* request执行失败后返回数据存到err */
    fail: (err) => {
      console.error('错误信息：', err); /* 调试窗口显示err内的内容 */
    }
  });
  },

  /* 开灯函数 */ 
  openLed:function(){
    /* 发送request请求 */ 
    wx.request({
       /* 官方调试API地址(具体看文档说明) */  
      url: 'https://iot-api.heclouds.com/thingmodel/set-device-property',
       /* request的操作方式(POST或者GET) */  
      method: 'POST',
      header: {
        /* 用户鉴权信息(使用token2.0生成，具体如word文档内说明) */  
        "authorization": "version=2022-05-01&res=userid%2F395514&et=1723879594&method=sha1&sign=r9OIGdid3W0EWWGgkfR7I%2BzQ15Y%3D"
      },
      /* 发送的body信息 */  
      data: {
        product_id:'4iXR2ZI4Tc',/* 产品ID */  
        device_name:'TemNHum',/* 设备名称 */  
        params:{
          "LED": true   /* 发送的数据（平台对应LED标识符修改为true）（注意数据类型不要传错） */ 
        }
      },
      /* request执行成功后返回数据存到res */ 
      success: res =>{
        console.log("成功",res)/* 调试窗口显示res内容 */ 
      }
    });
  },

  /* 关灯函数 */
  closeLed:function(){
    /* 发送request请求 */ 
    wx.request({
      /* 官方调试API地址(具体看文档说明)*/ 
      url: 'https://iot-api.heclouds.com/thingmodel/set-device-property',
      /* request的操作方式(POST或者GET) */  
      method: 'POST',
      header: {
        /* 用户鉴权信息(使用token2.0生成，具体如word文档内说明) */  
        "authorization": "version=2022-05-01&res=userid%2F395514&et=1723879594&method=sha1&sign=r9OIGdid3W0EWWGgkfR7I%2BzQ15Y%3D"
      },
      /* 发送的body信息 */  
      data: {
        product_id:'4iXR2ZI4Tc',  /* 产品ID */  
        device_name:'TemNHum',  /* 设备名称 */  
        params:{
          "LED": false   /* 发送的数据（平台对应LED标识符修改为true）（注意数据类型不要传错） */ 
        }
      },
      /* request执行成功后返回数据存到res */ 
      success: res =>{
        console.log("成功",res) /* 调试窗口显示res内容 */
      }
    });
  },

  /* 更新数据函数 */
  updatevalue:function(){
    /* 防止报错，先把页面this的内容传给that */
    var that = this;
    /* 发送request请求 */ 
    wx.request({
        /* 官方调试API地址(具体看文档说明)*/
        url: "https://iot-api.heclouds.com/thingmodel/query-device-property?product_id=4iXR2ZI4Tc&device_name=TemNHum",
        /* request的操作方式(POST或者GET) */ 
        method: "GET",
        header: {
          /* 用户鉴权信息(使用token2.0生成，具体如word文档内说明) */ 
          authorization: "version=2022-05-01&res=userid%2F395514&et=1723879594&method=sha1&sign=r9OIGdid3W0EWWGgkfR7I%2BzQ15Y%3D"
        },
        /* request执行成功后返回数据存到res */ 
        success:(res)=>{
          console.log("成功",res)
          if(res.data && res.data.data && res.data.data.length >= 4)  /* 判断返回的res内的内容符合要求 */
          {
            /* 调用setData去修改页面参数变量 */
            that.setData({
              "sensorslist[1].value":res.data.data[1].value,  /* data内数据为数组使用这种方法修改 */
              "sensorslist[0].value":res.data.data[3].value, /* data内数据为数组使用这种方法修改 */
              /*LedisOpen:res.data.data[2].value,*/
              chiptemperature_value:res.data.data[0].value, /* data内数据为普通变量使用这种方法修改 */
            })
          }
        },
        /* request执行失败后返回数据存到err */
        fail: (err) => {
          console.error('错误信息：', err); /* 调试窗口显示err内的内容 */
          /* 调用setData去修改页面参数变量 */
          that.data({
            isConnect:false,  /* data内数据为普通变量使用这种方法修改 */
          })
        }
      } 
    )
  },

/********************************/
/********************************/
/*** 防止定时器在后台还在死循环 ***/
/********************************/
/********************************/
  /* 页面隐藏时调用的函数 */
  onHide:function(){
    clearInterval(this.data.Interval);  /* 清空Interval数据 */
    clearTimeout(this.data.timeOut);  /* 清空Timeout数据 */
  },
  /* 页面重载时调用的函数 */
  onUnload:function(){
    clearInterval(this.data.Interval);  /* 清空Interval数据 */
    clearTimeout(this.data.timeOut);  /* 清空Timeout数据 */
  }
})
