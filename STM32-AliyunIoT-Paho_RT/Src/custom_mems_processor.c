#include "main.h"
#include "custom_bus.h"
#include "custom_errno.h"
#include "hts221.h"

HTS221_Object_t hts221_obj_0;

int32_t Sensor_Init(void)
{
  HTS221_IO_t            io_ctx;
  HTS221_Capabilities_t  cap;
  uint8_t                id;
  int32_t                ret = BSP_ERROR_NONE;
  
  /* Configure the environmental sensor driver */
  io_ctx.BusType     = HTS221_I2C_BUS; /* I2C */
  io_ctx.Address     = HTS221_I2C_ADDRESS;
  io_ctx.Init        = BSP_I2C1_Init;
  io_ctx.DeInit      = BSP_I2C1_DeInit;
  io_ctx.ReadReg     = BSP_I2C1_ReadReg;
  io_ctx.WriteReg    = BSP_I2C1_WriteReg;
  io_ctx.GetTick     = BSP_GetTick;

  if (HTS221_RegisterBusIO(&hts221_obj_0, &io_ctx) != HTS221_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (HTS221_ReadID(&hts221_obj_0, &id) != HTS221_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != HTS221_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    HTS221_GetCapabilities(&hts221_obj_0,&cap);
   /* Initialize the HTS221 sensor */
    if(HTS221_COMMON_Driver.Init(&hts221_obj_0)!= HTS221_OK)
      ret = BSP_ERROR_COMPONENT_FAILURE;
    else
      ret = BSP_ERROR_NONE;
    
    if(cap.Temperature==1)
    {
      if(HTS221_TEMP_Driver.Enable(&hts221_obj_0)!=HTS221_OK)
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    if(cap.Humidity==1)
    {
      if(HTS221_HUM_Driver.Enable(&hts221_obj_0)!=HTS221_OK)
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  
  return ret;  
}

/**
  * @brief  Handles the temperature data getting/sending
  * @param  Instance the device instance
  * @retval None
  */
void Temp_Sensor_Handler(void)
{
  float temperature;

  if((HTS221_TEMP_Driver.GetTemperature(&hts221_obj_0, &temperature))==0)
  {
    SetTemperatureValue((int8_t)temperature);
  }
}

/**
  * @brief  Handles the humidity data getting/sending
  * @param  Instance the device instance
  * @retval None
  */
void Hum_Sensor_Handler(void)
{
  float humidity;

  if ((HTS221_HUM_Driver.GetHumidity(&hts221_obj_0, &humidity))==0)
  {
    SetHumValue((uint8_t)humidity);
  }

}
