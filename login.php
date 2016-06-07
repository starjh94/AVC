<?php
include 'dbcon.php';
$ID = $_GET['id'];
$PASSWORD = $_GET['password'];
$query = "SELECT * FROM login";
$result = mysql_query($query);
while($row = mysql_fetch_array($result))
{
 $correct = 0;
 $dbid = $row['id'];
 $dbpassword = $row['password'];
 if($ID == $dbid)
 { 
  if($dbpassword == $PASSWORD)
  {
   $correct=1;
   break;
  }
 }
}
 if($correct == 0)
 {
 ?>
  <script language = "javascript">
  <!--
   window.location = "login.html";
   alert("아이디를 잘못 입력하셧습니다.");
  //-->
  </script>
  <?php
 }
 else 
  echo("로그인 완료");
 
?>