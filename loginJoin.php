<?php
include 'dbcon.php';
$ID = $_GET['id'];
$PASSWORD = $_GET['password'];
$query = "INSERT INTO login VALUE('$ID','$PASSWORD')";
mysql_query($query);
?>
<script language = "javascript">
window.location = "login.html"
alert = ("회원가입 완료. 다시 로그인 해주세요.");
</script>