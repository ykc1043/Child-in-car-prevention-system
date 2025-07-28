/* static/js/debug.js */
document.addEventListener("DOMContentLoaded", function(){
    // 更新 MQTT 连接状态
    function updateConnectionStatus() {
        fetch('/connection_status')
            .then(response => response.json())
            .then(data => {
                let statusElem = document.getElementById('connection-status');
                statusElem.textContent = data.status;
                if(data.status === "Connected"){
                    statusElem.classList.remove('badge-secondary');
                    statusElem.classList.add('badge-success');
                } else {
                    statusElem.classList.remove('badge-success');
                    statusElem.classList.add('badge-secondary');
                }
            })
            .catch(err => console.error(err));
    }
    updateConnectionStatus();
    setInterval(updateConnectionStatus, 2000);

    // 获取接收到的消息
    function fetchMessages() {
        var rxEncoding = document.querySelector('input[name="rx_encoding"]:checked').value;
        var rxMode = document.querySelector('input[name="rx_mode"]:checked').value;
        fetch('/get_messages?encoding=' + rxEncoding + '&mode=' + rxMode)
            .then(response => response.json())
            .then(data => {
                var showTimestamp = document.getElementById('rx_timestamp').checked;
                var rxBox = document.getElementById('rx-box');
                rxBox.value = "";
                data.forEach(function(msg){
                    if(showTimestamp){
                        rxBox.value += "[" + msg.timestamp + "] ";
                    }
                    rxBox.value += msg.payload + "\n";
                });
                rxBox.scrollTop = rxBox.scrollHeight;
            })
            .catch(err => console.error(err));
    }
    fetchMessages();
    setInterval(fetchMessages, 1000);

    // 清除消息
    document.getElementById('clear-button').addEventListener('click', function(){
        fetch('/clear_messages', {method: 'POST'})
            .then(response => response.json())
            .then(data => {
                if(data.success){
                    document.getElementById('rx-box').value = "";
                }
            });
    });

    // 发送消息
    document.getElementById('send-button').addEventListener('click', function(){
        var txBox = document.getElementById('tx-box');
        var txMessage = txBox.value.trim();
        if(!txMessage){
            alert("发送内容为空");
            return;
        }
        var txEncoding = document.querySelector('input[name="tx_encoding"]:checked').value;
        var txMode = document.querySelector('input[name="tx_mode"]:checked').value;
        fetch('/send_message', {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({
                message: txMessage,
                encoding: txEncoding,
                mode: txMode
            })
        })
        .then(response => response.json())
        .then(data => {
            if(data.success){
                txBox.value = "";
            } else {
                alert("发送失败: " + data.error);
            }
        })
        .catch(err => {
            alert("发送请求失败");
        });
    });
});
