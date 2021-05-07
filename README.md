# Introduction:
This project turns an Arduino into an interval timer which measures the time elapsed between two pulses. It uses the RPC protocol which allows the user to configure and read out the data via the CrystalTestFramework.

RPC Protocol:
[https://github.com/Crystal-Photonics/RPC-Generator](https://github.com/Crystal-Photonics/RPC-Generator)

CrystalTestFramework:
[https://github.com/Crystal-Photonics/crystalTestFrameworkApp](https://github.com/Crystal-Photonics/crystalTestFrameworkApp)


# Usage:

The project is built to be used via the CrystalTestFramework(CTFW). The CTFW connects to the Arduino via the RPC protocol. Using the .xml file which is generated by the RPC generator while building the Arduino software, the CTFW knows the exposed functions of the Arduino and integrates them into the Lua scripting interface. Following functions and types are available from the Lua script running within the CTFW:

```C
typedef struct {
    uint32_t githash;
    uint32_t gitDate_unix;
    uint32_t serialnumber;
    uint16_t deviceID;
    uint8_t guid[12];
    uint8_t boardRevision;
    char name[12];
    char version[8];
} device_descriptor_v1_t;

device_descriptor_v1_t get_device_descriptor(void)
```

```C
typedef enum {
    d0_roundstop,
    d1_roundstop,
    d2_roundstop,
    d3_roundstop,
    d4_roundstop,
    d5_roundstop,
    d6_roundstop,
    d7_roundstop
} arduino_dig_pin_roundstop_t;

typedef enum { d2_trigger, d3_trigger } arduino_dig_pin_trigger_t;
typedef enum { edge_rising, edge_falling } arduino_dig_edge_t;

//defines the condition to stop the timer for the corresponding channel. If the timer
//is running and the condition is met the current timer value is captured and stored
//into the corresponding channel.
void define_roundstop_pin(arduino_dig_pin_roundstop_t pin, arduino_dig_edge_t edge);

//defines the condition to start the timer. After this function being called the
//Arduino waits until the right edge on the right pin happens and starts and resets
//the timer
void start_timer(arduino_dig_pin_trigger_t triggered_by, arduino_dig_edge_t edge);

```

```C
typedef struct {
    uint16_t round_times_64us[8];
    uint8_t triggered[8];
} round_times_t;

//returns the captured timer values for each channel in 64us. To get milliseconds try
// x_ms = (t_64us[i]/0.064)/1000.0
// or simpler: x_ms = t_64us[i]/64.0
round_times_t get_round_times(void);
```

```C
//resets everything(also configuration)
void reset_all();

//just resets the timer values but keeps the configuration
void reset_times_only()

//disables timer
void stop_timer();

```


# Example
 
In this Example the ArduWatch and the function generator HM8150 are used to measure the delay between two events. First the ArduWatch is reset and programmed to be triggered by an falling edge on pin d2. The delays between the edges on d2 and d4 and the delays between d2 and d3 are measured. After a timeout the timer is stopped and the captured data is retrieved by arduwatch:get_round_times(). This data is converted to milliseconds and put into a histogram plot. The function generator is used to feed a signal into the device under test at the right moment.

```lua
device_requirements =
{
    {
        protocol = "RPC",
        device_names = {"ArduWatch"},
        quantity = 1, --or "each_matching",
        alias = "arduwatch"
    },{
        protocol = "SCPI",
        device_names = {"HM8150"},
        quantity = 1,
        alias = "funktionsgenerator"
    }
}


function run(devices)
    local arduwatch = devices.arduwatch
    local funktionsgenerator = devices.funktionsgenerator
   
    funktionsgenerator:send_command("OT0") --Funktionsgenerator Ausgang aus
    funktionsgenerator:send_command("OF0") --Funktionsgenerator Offset aus

    local plot = Ui.Plot.new()
    local curves = {}
    curves[1] = plot:add_curve()
    curves[2] = plot:add_curve()
    curves[1]:set_color(Ui.Color("black"))
    curves[2]:set_color(Ui.Color("red"))
   
    arduwatch:reset_all()
    arduwatch:define_roundstop_pin(arduwatch.arduino_dig_pin_roundstop_t.d4_roundstop, arduwatch.arduino_dig_edge_t.edge_rising)
    arduwatch:define_roundstop_pin(arduwatch.arduino_dig_pin_roundstop_t.d3_roundstop, arduwatch.arduino_dig_edge_t.edge_falling)

    local stats_max_ms = {0,0};
    local stats_avg_ms = {0,0};
    local stats_sum_ms = {0,0};
    local stats_min_ms = {100000,100000};
    local count = 0;
    local HISTOGRAM_LENGTH = 350   
    while(true) do
        local histograms = {{},{}}
        for i = 1, #histograms do
            for n = 1, HISTOGRAM_LENGTH do
                histograms[i][n] = 0
            end   
        end

        funktionsgenerator:send_command("OT0") --Funktionsgenerator Ausgang aus
        sleep_ms(100)
        arduwatch:start_timer(arduwatch.arduino_dig_pin_trigger_t.d2_trigger, arduwatch.arduino_dig_edge_t.edge_falling);
        sleep_ms(100)
        funktionsgenerator:send_command("OT1") --Funktionsgenerator Ausgang ein
        sleep_ms(500)
        funktionsgenerator:send_command("OT0") --Funktionsgenerator Ausgang aus
        sleep_ms(1000)
        arduwatch:stop_timer()
        local round_times_raw = arduwatch:get_round_times()
        arduwatch:reset_times_only()
        local round_times_ms = {}
        for i=1,8 do
            if round_times_raw.triggered[i] == 1 then
                print(round_times_raw.round_times_64us[i])
                round_times_ms[i] = round((round_times_raw.round_times_64us[i]/0.064)/1000.0,1)
            end
        end
       
        local append_data = {os.time()}
        append_data[2] = round_times_ms[5]
        append_data[3] = round_times_ms[4]
        if append_data[2] ~= nil or append_data[2] ~= nil then
            print("current:")
            print(append_data)
            count = count + 1
            for i = 2, #append_data do
                if append_data[i] > stats_max_ms[i-1] then
                    stats_max_ms[i-1] = append_data[i]
                end
                if append_data[i] < stats_min_ms[i-1] then
                    stats_min_ms[i-1] = append_data[i]
                end
                stats_sum_ms[i-1] = stats_sum_ms[i-1] + append_data[i]
                stats_avg_ms[i-1] = stats_sum_ms[i-1] / count
               
                local rounded_val = round(append_data[i],0)
                if rounded_val == 0 then
                    rounded_val = 1
                end
                if rounded_val > #histograms[i-1] then
                    rounded_val = #histograms[i-1]
                end
                histograms[i-1][rounded_val] = histograms[i-1][rounded_val] + 1
                curves[i-1]:add_spectrum_at(0,histograms[i-1])
            end
            if true then
                print("max:")
                print(stats_max_ms)
                print("min:")
                print(stats_min_ms)
                print("avg:")
                print(stats_avg_ms)
                print("")
            end
        end
    end
end
```
