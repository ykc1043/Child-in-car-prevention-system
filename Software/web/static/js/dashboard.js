/* static/js/dashboard.js */
document.addEventListener("DOMContentLoaded", function () {
    // 全局变量：记录最近一次成功接收数据的时间（Web 服务成功）和收到新有效 MQTT 数据的时间
    let lastWebSuccessTime = null;  // 每次 HTTP 调用成功都更新（无论数据是否新）
    let lastDataTime = null;        // 当收到新有效 MQTT 数据（即 ts 字段变化）时更新
    let lastReceivedTs = null;      // 记录上一次返回的最新数据的时间戳（字符串格式，例如 "12:34:56"）

    // 尝试获取 Dashboard 数据
    function updateDashboard() {
        fetch('/get_dashboard_data')
            .then(response => {
                if (!response.ok) {
                    throw new Error("网络响应异常");
                }
                return response.json();
            })
            .then(data => {
                let now = new Date();
                // 更新 Web 服务成功时间
                lastWebSuccessTime = now;
                // 更新显示区域（无论数据是否新，都更新显示）
                document.getElementById('temphum-display').textContent = `${data.t} °C, ${data.h} %`;
                document.getElementById('co2tvoc-display').textContent = `${data.c} ppm, ${data.v} ppb`;
                document.getElementById('geo-display').textContent = `经度: ${data.lng}, 纬度: ${data.lat}`;
                // 若历史数据数组不为空，则取最新一条的 ts 作为数据时间戳
                if (data.temphum_history && data.temphum_history.length > 0) {
                    let newTs = data.temphum_history[data.temphum_history.length - 1].ts;
                    // 当最新的 ts 与上一次不同，则认为有新数据
                    if (newTs !== lastReceivedTs) {
                        lastReceivedTs = newTs;
                        lastDataTime = now;
                    }
                }
            })
            .catch(err => {
                console.error(err);
                // 请求失败时，不更新 lastWebSuccessTime 和 lastDataTime
            });
    }
    // 每2秒请求一次数据
    updateDashboard();
    setInterval(updateDashboard, 2000);

    // 每秒更新状态徽章和“最后更新”提示
    function updateStatus() {
        const now = new Date();
        // 判断 Web 服务状态：10秒内有成功请求即为正常
        let webNormal = (lastWebSuccessTime && (now - lastWebSuccessTime) < 10000);
        // 判断数据状态：10秒内有收到新有效数据即为正常
        let dataNormal = (lastDataTime && (now - lastDataTime) < 10000);

        // 更新徽章样式
        const badgeWeb = document.getElementById("status-web");
        const badgeData = document.getElementById("status-data");
        if (webNormal) {
            badgeWeb.textContent = "S";
            badgeWeb.style.backgroundColor = "green";
        } else {
            badgeWeb.textContent = "S";
            badgeWeb.style.backgroundColor = "red";
        }
        if (dataNormal) {
            badgeData.textContent = "D";
            badgeData.style.backgroundColor = "green";
        } else {
            badgeData.textContent = "D";
            badgeData.style.backgroundColor = "red";
        }
        // 最后更新显示：以 lastDataTime 为准（如果不存在，则使用 lastWebSuccessTime）
        let lastUpdate = lastDataTime ? lastDataTime : lastWebSuccessTime;
        if (lastUpdate) {
            let diff = Math.floor((now - lastUpdate) / 1000);
            if (diff > 3600) {
                diff = 3600;
            } else if (diff < 10) {
                document.getElementById("update-time").textContent = "几秒前";
            } else{
                document.getElementById("update-time").textContent = diff + "秒前";
            }

        } else {
            document.getElementById("update-time").textContent = "--";
        }
    }
    setInterval(updateStatus, 1000);

    // 以下部分为历史趋势图逻辑（保持不变）
    var temphumChart;
    var co2tvocChart;

    function updateTemphumChart(historyData) {
        var ctx = document.getElementById('chart-temphum').getContext('2d');
        var labels = historyData.map(item => item.ts);
        var tempData = historyData.map(item => item.t);
        var humData = historyData.map(item => item.h);

        if (temphumChart) {
            temphumChart.data.labels = labels;
            temphumChart.data.datasets[0].data = tempData;
            temphumChart.data.datasets[1].data = humData;
            temphumChart.update();
        } else {
            temphumChart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: '温度 (°C)',
                        data: tempData,
                        borderColor: 'rgba(255, 99, 132, 1)',
                        fill: false
                    }, {
                        label: '湿度 (%)',
                        data: humData,
                        borderColor: 'rgba(54, 162, 235, 1)',
                        fill: false
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false
                }
            });
        }
    }

    function updateCo2tvocChart(historyData) {
        var ctx = document.getElementById('chart-co2tvoc').getContext('2d');
        var labels = historyData.map(item => item.ts);
        var co2Data = historyData.map(item => item.c);
        var tvocData = historyData.map(item => item.v);

        if (co2tvocChart) {
            co2tvocChart.data.labels = labels;
            co2tvocChart.data.datasets[0].data = co2Data;
            co2tvocChart.data.datasets[1].data = tvocData;
            co2tvocChart.update();
        } else {
            co2tvocChart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: 'CO2 (ppm)',
                        data: co2Data,
                        borderColor: 'rgba(75, 192, 192, 1)',
                        fill: false
                    }, {
                        label: 'TVOC (ppb)',
                        data: tvocData,
                        borderColor: 'rgba(153, 102, 255, 1)',
                        fill: false
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false
                }
            });
        }
    }

    // 点击卡片时加载对应的历史趋势图
    document.getElementById('card-temphum').addEventListener('click', function () {
        fetch('/get_dashboard_data')
            .then(response => response.json())
            .then(data => {
                updateTemphumChart(data.temphum_history);
                $('#modal-temphum').modal('show');
            });
    });

    document.getElementById('card-co2tvoc').addEventListener('click', function () {
        fetch('/get_dashboard_data')
            .then(response => response.json())
            .then(data => {
                updateCo2tvocChart(data.co2tvoc_history);
                $('#modal-co2tvoc').modal('show');
            });
    });

    document.getElementById('card-geo').addEventListener('click', function () {
        fetch('/get_dashboard_data')
            .then(response => response.json())
            .then(data => {
                var lng = data.lng;
                var lat = data.lat;
                window.location.href = `/map?lng=${lng}&lat=${lat}`;
            });
    });
});
