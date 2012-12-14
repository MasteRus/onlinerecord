<?php
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */



function onlinerecord_orgs_form_add(&$form_state)
{
   $form['neworganization']= array(
      '#title' => t('Insert Organization name'),
      '#type' => 'textfield',
      '#default_value' => '',
    );

   $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add'),
    );
    return $form;
}
function onlinerecord_orgs_form_add_submit($form, &$form_state)
{
    //$str=check_plain($form_state['values']['neworganization']);
    $str=$form_state['values']['neworganization'];
    $result=db_query("INSERT INTO {onlinerecord_organizations} (name) VALUES ('%s')",$str);
    drupal_set_message('result='.$result);
}

function onlinerecord_orgs_form_edit(&$form_state)
{
  $arg=check_plain(arg(4));
  $result=db_query ('Select name FROM {onlinerecord_organizations} where org_id=%d',$arg );
  $org=db_fetch_array($result) ;

  //$form['org_id']=$arg;
  //drupal_set_message('array orgs=<PRE>'.check_plain(print_r($org,1)).'<PRE>', 'error');
  if ($org['name']!='')
  {
    $form['neworganization']= array(
      '#title' => t('Insert Organization name'),
      '#type' => 'textfield',
      '#default_value' => $org['name'],
    );


    $form['org_id']=array(
      '#type' => 'value',
      '#value' => $arg,
    );



    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );
  }
  else {$form=drupal_get_form('onlinerecord_orgs_list');}
  return $form;
}
function onlinerecord_orgs_form_edit_submit($form, &$form_state)
{
    $str=$form_state['values']['neworganization'];
    $org_id=$form_state['values']['org_id'];
    $result=db_query("UPDATE {onlinerecord_organizations} set name='%s' where org_id=%d",$str,$org_id);
    //drupal_set_message('array orgs=<PRE>'.check_plain(print_r($form_state['values'],1)).'<PRE>', 'error');
    //drupal_set_message('result='.$result);
}
function onlinerecord_orgs_form_delete_confirm(&$form_state)
{
    $message=t('Are you sure want to delete organization?');
    $arg=check_plain(arg(4));

    $result=db_query("Select name FROM {onlinerecord_organizations} WHERE org_id=%d",$arg);

    $org = db_fetch_object($result);

    $form['org_id']=array(
      '#type' => 'value',
      '#value' => $arg,
    );
    $form['name']=array(
      '#type' => 'value',
      '#value' => $org->name,
    );
    
    //drupal_set_message('array orgs=<PRE>'.(print_r($GLOBALS,1)).'<PRE>', 'error');
    return confirm_form(
      $form,
      $message,
      'admin/settings/onlinerecord/orgs',
      '<i>this operation could not be canceled!</i>',
      'Delete',
      'Cancel'
    );
}

function onlinerecord_orgs_form_delete_confirm_submit($form, &$form_state)
{
    if ($form_state['values']['confirm']) {
      $org_id=$form_state['values']['org_id'];
      $name=$form_state['values']['name'];
      db_query("DELETE FROM {onlinerecord_organizations} WHERE org_id=%d",$org_id);
      drupal_set_message('The organization <i>'.$name.'</i> was deleted','status');
      $form_state['redirect']='admin/settings/onlinerecord/orgs/';
      //@TODO - Department, plan, and records in online_record controls
    }
}


function onlinerecord_orgs_list()
{
    $result=db_query('Select org_id, name FROM {onlinerecord_organizations}');

    $tableHeader = array('org_id', 'name', 'Actions');

    //$tableHeader = array('org_id', 'name');
    $tableData = array();
    
    while ($org = db_fetch_object($result))
    {
        $actions  = l('изменить', 'admin/settings/onlinerecord/orgs/' . $org->org_id . '/edit/', array('alias' => true)) . ' | ';
        $actions .= l('удалить', 'admin/settings/onlinerecord/orgs/' . $org->org_id. '/delete/', array('alias' => true));

        $tableData[] = array(
            $org->org_id,
            $org->name,
            $actions,
        );
    }
    
    return theme('table', $tableHeader, $tableData);
    //return $form;
}

/////////////////////////////////////////////////////////////////////////////////

function onlinerecord_depts_form_add(&$form_state)
{
   $result=db_query('Select org_id, name FROM {onlinerecord_organizations}');
   $orgs=array();
   $orgs[0]='---';
   $i=0;
   while ($org=db_fetch_array($result)) {
      $i=intval($org['org_id']);
      $orgs[$i]=$org['name'];
   }

   $form['newdept']= array(
      '#title' => t('Insert Department name'),
      '#type' => 'textfield',
      '#default_value' => '',
    );

   $form['parent_org_id'] = array(
      '#title' => t('Select your place'),
      '#type' => 'select',
      '#options' => $orgs,
      '#default_value' => 0,
   );

   $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add'),
    );

    return $form;
}

function onlinerecord_depts_form_add_submit($form, &$form_state)
{
    //$str=check_plain($form_state['values']['neworganization']);
    $name=$form_state['values']['newdept'];
    $org_id=$form_state['values']['parent_org_id'];
    $result=db_query("INSERT INTO {onlinerecord_departments} (org_id,name) VALUES ('%d','%s')",intval($org_id),$name);
    //drupal_set_message('result='.$result);
}

function onlinerecord_depts_form_edit(&$form_state)
{
  $arg=check_plain(arg(4));
  $result=db_query ('Select org_id, name FROM {onlinerecord_departments} where dept_id=%d',$arg );
  $dept=db_fetch_array($result) ;

  //$org=$dept['ord_id'];

  if ($dept['name']!='')
  {
    // Creating List of organizations for you parent_org select widget
    $result=db_query("Select org_id, name FROM {onlinerecord_organizations}");
    $orgs=array();
    $orgs[0]='---';
    $i=0;
    while ($org=db_fetch_array($result)) {
      $i=intval($org['org_id']);
      $orgs[$i]=$org['name'];
    }

    // name text widget
    $form['deptname']= array(
      '#title' => t('Insert Organization name'),
      '#type' => 'textfield',
      '#default_value' => $dept['name'],
    );

    // id-value
    $form['dept_id']=array(
      '#type' => 'value',
      '#value' => $arg,
    );

    // parent_org_id value
    $form['parent_org_id'] = array(
      '#title' => t('Select your place'),
      '#type' => 'select',
      '#options' => $orgs,
      '#default_value' => $dept['org_id'],
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add'),
    );
    return $form;
  }
  else {$form=drupal_get_form('onlinerecord_depts_list');}
  return $form;
}

function onlinerecord_depts_form_edit_submit($form, &$form_state)
{

    $str=$form_state['values']['deptname'];
    $dept_id=$form_state['values']['dept_id'];
    $org_id=$form_state['values']['parent_org_id'];
    $result=db_query("UPDATE {onlinerecord_departments} set name='%s', org_id=%d where dept_id=%d",$str,$org_id,$dept_id);
    //drupal_set_message('array orgs=<PRE>'.check_plain(print_r($form_state['values'],1)).'<PRE>', 'error');
    //drupal_set_message('result='.$result);
}


function onlinerecord_depts_form_delete_confirm(&$form_state)
{
    $message=t('Are you sure want to delete department?');
    $arg=check_plain(arg(4));

    $result=db_query("Select name FROM {onlinerecord_departments} WHERE dept_id=%d",$arg);

    $org = db_fetch_object($result);

    $form['dept_id']=array(
      '#type' => 'value',
      '#value' => $arg,
    );
    $form['name']=array(
      '#type' => 'value',
      '#value' => $org->name,
    );

    //drupal_set_message('array orgs=<PRE>'.(print_r($GLOBALS,1)).'<PRE>', 'error');
    return confirm_form(
      $form,
      $message,
      'admin/settings/onlinerecord/depts',
      '<i>this operation could not be canceled!</i>',
      'Delete',
      'Cancel'
    );
}

function onlinerecord_depts_form_delete_confirm_submit($form, &$form_state)
{
    if ($form_state['values']['confirm']) {
      $dept_id=$form_state['values']['dept_id'];
      $name=$form_state['values']['name'];
      db_query("DELETE FROM {onlinerecord_departments} WHERE dept_id=%d",$dept_id);
      drupal_set_message('The department <i>'.$name.'</i> was deleted','status');
      $form_state['redirect']='admin/settings/onlinerecord/depts/';
      //@TODO - Plan, and records in online_record controls
    }
}


function onlinerecord_depts_list()
{
    $result=db_query('SELECT d.dept_id, d.name as deptname, o.name as orgname  FROM {onlinerecord_departments} d INNER JOIN {onlinerecord_organizations} o ON d.org_id=o.org_id');
    $tableHeader = array('dept_id', 'name','organization' ,'Actions');
    $tableData = array();

    while ($org = db_fetch_object($result))
    {
        $actions  = l('изменить', 'admin/settings/onlinerecord/depts/' . $org->dept_id. '/edit/', array('alias' => true)) . ' | ';
        $actions .= l('удалить', 'admin/settings/onlinerecord/depts/' . $org->dept_id. '/delete/', array('alias' => true));

        $tableData[] = array(
            $org->dept_id,
            $org->deptname,
            $org->orgname,
            $actions,
        );
    }
    return theme('table', $tableHeader, $tableData);
}
////////////////////////////////////////////////////////////////////////////////

function onlinerecord_plans_form_add(&$form_state)
{
   

   // Organizations with depts
   $result=db_query('Select d.dept_id, d.name as dname, o.name AS oname FROM {onlinerecord_organizations} o, {onlinerecord_departments} d
   WHERE d.org_id=o.org_id');

   $orgs=array();
   $orgs[0]='---';
   $i=0;
   while ($org=db_fetch_array($result)) {
      $i=intval($org['dept_id']);
      $orgs[$i]=$org['oname'].'|'.$org['dname'];
   }

  

   $form['parent_dept_id'] = array(
      '#title' => t('Select your place'),
      '#type' => 'select',
      '#options' => $orgs,
      '#default_value' => 0,
   );

   $form['plan_name']= array(
      '#title' => t('Insert Plan name'),
      '#type' => 'textfield',
      '#default_value' => '',
    );

   $values=array(4=>4,5=>5,6=>6,8=>8,10=>10,15=>15,20=>20,30=>30);
   $form['kvant'] = array(
      '#title' => t('Select your place'),
      '#type' => 'select',
      '#options' => $values,
      '#default_value' => 0,
   );

   $form['worktimestart'] = array(
      '#title' => t('Start time'),
      '#type' => 'textfield',
      '#default_value' => '',
   );

   $form['worktimefinish'] = array(
      '#title' => t('Finish time'),
      '#type' => 'textfield',
      '#default_value' => '',
   );

   $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add'),
    );
   
   return $form;
}
function onlinerecord_plans_form_add_submit($form, &$form_state)
{
    //$str=check_plain($form_state['values']['neworganization']);
    $name=$form_state['values']['plan_name'];
    $kvant=intval($form_state['values']['kvant']);
    $dept_id=intval($form_state['values']['parent_dept_id']);
    
    $wts=intval($form_state['values']['worktimestart']);
    $wtf=intval($form_state['values']['worktimefinish']);

    $result=db_query("INSERT INTO {onlinerecord_plans}
      (name,dept_id, kvant_value,worktimestart,worktimefinish)
      VALUES ('%s',%d,%d,%d,%d)",$name,$dept_id,$kvant,$wts,$wtf);
    drupal_set_message('result='.$result);
}

function onlinerecord_plans_form_edit(&$form_state)
{
  $arg=check_plain(arg(4));
  $result=db_query('SELECT plan_id, dept_id, name, kvant_value, worktimestart, worktimefinish
       FROM {onlinerecord_plans} where plan_id=%d',$arg);
  $plan=db_fetch_array($result) ;

  
  if ($plan['name']!='')
  {

    $result=db_query('Select d.dept_id, d.name as dname, o.name AS oname FROM {onlinerecord_organizations} o, {onlinerecord_departments} d
   WHERE d.org_id=o.org_id');

   $orgs=array();
   $orgs[0]='---';
   $i=0;
   while ($org=db_fetch_array($result)) {
      $i=intval($org['dept_id']);
      $orgs[$i]=$org['oname'].'|'.$org['dname'];
   }

    // id-value
   $form['plan_id']=array(
     '#type' => 'value',
     '#value' => $arg,
   );

   $form['parent_dept_id'] = array(
      '#title' => t('Select your place'),
      '#type' => 'select',
      '#options' => $orgs,
      '#default_value' => $plan['dept_id'],
   );

   $form['plan_name']= array(
      '#title' => t('Insert Plan name'),
      '#type' => 'textfield',
      '#default_value' => $plan['name'],
    );

   //$values=array(4,5,6,8,10,15,20,30);
   $values=array(4=>4,5=>5,6=>6,8=>8,10=>10,15=>15,20=>20,30=>30);
   $form['kvant'] = array(
      '#title' => t('Select your place'),
      '#type' => 'select',
      '#options' => $values,
      '#default_value' => $plan['kvant_value'],
   );

   $form['worktimestart'] = array(
      '#title' => t('Start time'),
      '#type' => 'textfield',
      '#default_value' =>$plan['worktimestart'],
   );

   $form['worktimefinish'] = array(
      '#title' => t('Finish time'),
      '#type' => 'textfield',
      '#default_value' => $plan['worktimefinish'],
   );

   $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add'),
    );
  }
  else {$form=drupal_get_form('onlinerecord_depts_list');}
  return $form;
}

function onlinerecord_plans_form_edit_submit($form, &$form_state)
{
    $name=$form_state['values']['plan_name'];
    $kvant=intval($form_state['values']['kvant']);
    $dept_id=intval($form_state['values']['parent_dept_id']);

    $wts=intval($form_state['values']['worktimestart']);
    $wtf=intval($form_state['values']['worktimefinish']);

    $plan_id=$form_state['values']['parent_dept_id'];

    $result=db_query("UPDATE {onlinerecord_plans} SET name='%s', dept_id=%d, kvant_value=%d, worktimestart=%d, worktimefinish=%d
    WHERE plan_id=%d",
    $name,$dept_id,$kvant,$wts,$wtf,
    $plan_id);

}

function onlinerecord_plans_form_delete_confirm(&$form_state)
{
    $message=t('Are you sure want to delete plan?');
    $arg=check_plain(arg(4));

    $result=db_query("Select name FROM {onlinerecord_plans} WHERE plan_id=%d",$arg);

    $org = db_fetch_object($result);

    $form['plan_id']=array(
      '#type' => 'value',
      '#value' => $arg,
    );
    $form['name']=array(
      '#type' => 'value',
      '#value' => $org->name,
    );

    //drupal_set_message('array orgs=<PRE>'.(print_r($GLOBALS,1)).'<PRE>', 'error');
    return confirm_form(
      $form,
      $message,
      'admin/settings/onlinerecord/depts',
      '<i>this operation could not be canceled!</i>',
      'Delete',
      'Cancel'
    );
}

function onlinerecord_plans_form_delete_confirm_submit($form, &$form_state)
{
    if ($form_state['values']['confirm']) {
      $dept_id=$form_state['values']['plan_id'];
      $name=$form_state['values']['name'];
      db_query("DELETE FROM {onlinerecord_plans} WHERE plan_id=%d",$plan_id);
      drupal_set_message('The department <i>'.$name.'</i> was deleted','status');
      $form_state['redirect']='admin/settings/onlinerecord/plans/';
      //@TODO - Records in online_record controls
    }
}

function onlinerecord_plans_list()
{
    //@TODO - New field DisplayName or innerjoin?
    $result=db_query('SELECT p.plan_id, p.name AS planname, p.kvant_value, p.worktimestart, p.worktimefinish,
    d.name AS deptname, o.name AS orgname  FROM {onlinerecord_plans} p, {onlinerecord_organizations} o, {onlinerecord_departments} d
    WHERE p.dept_id=d.dept_id AND d.org_id=o.org_id');

    $tableHeader = array('plan_id', 'org-dept', 'name', 'kvant_value', 'worktimestart', 'worktimefinish', 'Actions');

    $tableData = array();

    while ($plan = db_fetch_object($result))
    {
        $actions  = l('изменить', 'admin/settings/onlinerecord/plans/' . $plan->plan_id . '/edit/', array('alias' => true)) . ' | ';
        $actions .= l('удалить', 'admin/settings/onlinerecord/plans/' . $plan->plan_id. '/delete/', array('alias' => true));

        $tableData[] = array(
            $plan->plan_id,
            $plan->deptname." ".$plan->orgname,
            $plan->planname,
            $plan->kvant_value,
            $plan->worktimestart,
            $plan->worktimefinish,
            $actions,
        );
    }

    return theme('table', $tableHeader, $tableData);
    //return $form;
}

////////////////////////////////// Exceptions //////////////////////////////////
// @TODO Do everymonth, everyyear exceptions (#3 and #4)
// We do only everyday, everyweek and custom types (#1,#2 and #5) So try to

function onlinerecord_exceptions_per_plan_add(&$form_state)
{
   $arg=check_plain(arg(4));
   $result=db_query('SELECT kvant_value, worktimestart, worktimefinish FROM {onlinerecord_plans}
   WHERE plan_id=%d', $arg);

   $plan = db_fetch_object($result);
     $kvant_value = $plan->kvant_value;
     $wts = $plan->worktimestart;
     $wtf = $plan->worktimefinish;
   
   $form=array();
   $form['plan_id']=array(
      '#type' => 'value',
      '#value' => $arg,
   );
   $form['kvant_value']=array(
      '#type' => 'value',
      '#value' => $kvant_value,
   );
   $form['wts']=array(
      '#type' => 'value',
      '#value' => $wts,
   );
   $form['wtf']=array(
      '#type' => 'value',
      '#value' => $wtf,
   );

   $types_of_exception=array();
   $types_of_exception[0]='---';
   $types_of_exception[1]='everyday';
   $types_of_exception[2]='everyweek';
   $types_of_exception[3]='everymonth';
   $types_of_exception[4]='everyyear';
   $types_of_exception[5]='custom';

   $form['type_of_exception'] = array(
      '#title' => t('Type of exception'),
      '#type' => 'select',
      '#options' => $types_of_exception,
      '#default_value' => 0,
   );

   //Set number of quants
   $n=($wtf-$wts)/($kvant_value*60);

   $kvantn=array();
   $kvantn[-1]="---";

   for ($i=0;$i<$n;$i++)
   {
     $temp=$wts+$i*$kvant_value*60;
     $kvantn[$i]=strval((int)($temp/3600)).":".//Hours
       str_pad(strval((int)(($temp%3600)/60)),2,'0',STR_PAD_LEFT);     //minutes with leading NULLs;
   }
   $form['kvant_start'] = array(
     '#title' => t('Start quant'),
     '#type' => 'select',
     '#options' => $kvantn,
     '#default_value' => -1,
     );

   $form['kvant_finish'] = array(
     '#title' => t('Finish quant'),
     '#type' => 'select',
     '#options' => $kvantn,
     '#default_value' => -1,
     );
   //if type=2 (everyweek) or 3 (everymonth)
   //then we need a daynumber
   $form['daynumber'] = array(
      '#title' => t('Daynumber'),
      '#type' => 'select',
      '#options' => array(0=>' ',1=>'1',2=>'2',3=>'3',4=>'4',5=>'5',6=>'6',7=>'7'),
      '#default_value' => 0,
   );// select?


   $dateformat='Y-m-d';
   $datef=date($dateformat);
   
   //if type=4 (everyyear) or 5 (custom)
   //then we need a daynumber
   $form['dateofexception'] = array(
      '#title' => t('Date of Exception'),
      '#type' => 'date_select',
      '#default_value' => $datef,
      '#date_format' => $dateformat,
      '#date_timezone' => 'Asia/Novosibirsk',
      '#date_increment' => $kvant_value,
      '#date_year_range' => '0:+1',
   );

   $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add'),
    );

   return $form;
}
function onlinerecord_exceptions_per_plan_add_validate($form, &$form_state)
{
    $plan_id=intval($form_state['values']['plan_id']);
    $kv=intval($form_state['values']['kvant_value']);
    $wts=intval($form_state['values']['wts']);
    $wtf=intval($form_state['values']['wtf']);

    $type_of_exception =intval($form_state['values']['type_of_exception']);
    $kvant1=intval($form_state['values']['kvant_start']);
    $kvant2=intval($form_state['values']['kvant_finish']);

    $dn=intval($form_state['values']['daynumber']);
    $dateofe=$form_state['values']['dateofexcept'];

    if ($kvant2<$kvant1)
      form_set_error('kvant_start',t('kvant_start > kvant_finish'));

    if ((($type_of_exception)==2)&&($dn==0) )
    {
      form_set_error('daynumber',t('Daynumber must be setted correctly'));
    };

    if ((($type_of_exception)==5)&&($dateofe=='') )
    {
          form_set_error('dateofexcept',t('Date must be setted correctly'));
    };


}


function onlinerecord_exceptions_per_plan_add_submit($form, &$form_state)
{
    $plan_id=intval($form_state['values']['plan_id']);
    $kv=intval($form_state['values']['kvant_value']);
    $wts=intval($form_state['values']['wts']);
    $wtf=intval($form_state['values']['wtf']);

    $type_of_exception=intval($form_state['values']['type_of_exception']);
    $kvant1=intval($form_state['values']['kvant_start']);
    $kvant2=intval($form_state['values']['kvant_finish']);

    $dn=intval($form_state['values']['daynumber']);
    $dateofe=$form_state['values']['dateofexcept'];

    switch($type_of_exception) {
      case 0: //error - type not selected
        break;
      case 1://everyday
        $result=db_query("INSERT INTO {onlinerecord_exceptions}
          (plan_id,type,time_start,time_stop)
          VALUES (%d,%d,%d,%d)",$plan_id,$type_of_exception,
            $wts+$kvant1*$kv*60,// Start time
            $wts+$kvant2*$kv*60);// Finish time
        break;
      case 2://everyweek we need correct daynumber (1-Monday, 7-Sunday)
        $result=db_query("INSERT INTO {onlinerecord_exceptions}
          (plan_id,type,time_start,time_stop,daynumber)
          VALUES (%d,%d,%d,%d,%d)",$plan_id,$type_of_exception,
            $wts+$kvant1*$kv*60,
            $wts+$kvant2*$kv*60,
            $dn);
        break;
      case 3: // @TODO everymonth
        break;
      case 4: // @TODO everyyear
        break;
      case 5://everyweek we need correct dateofexception (1-Monday, 7-Sunday)
        $result=db_query("INSERT INTO {onlinerecord_exceptions}
          (plan_id,type,time_start,time_stop,daynumber)
          VALUES (%d,%d,%d,%d,'%s')",$plan_id,$type_of_exception,
            $wts+$kvant1*$kv*60,// Start time
            $wts+$kvant2*$kv*60,// Finish time
            $dateofe);
        break;
    }

    drupal_set_message('result='.$result);
}

////////////////////

function onlinerecord_exceptions_edit(&$form_state)
{
   $arg=check_plain(arg(4));
   $result=db_query('SELECT e.type, e.daynumber, e.dateofexcept, e.time_start, e.time_stop,
    p.kvant_value, p.worktimestart, p.worktimefinish FROM {onlinerecord_plans} p, {onlinerecord_exceptions} e
    WHERE e.except_id=%d AND e.plan_id=p.plan_id', $arg);

   $except = db_fetch_object($result);
     $kvant_value = $except->kvant_value;
     $wts = $except->worktimestart;
     $wtf = $except->worktimefinish;

   $form=array();

   $form['except_id']=array(
      '#type' => 'value',
      '#value' =>  $arg,
   );
   $form['kvant_value']=array(
      '#type' => 'value',
      '#value' => $kvant_value,
   );
   $form['wts']=array(
      '#type' => 'value',
      '#value' => $wts,
   );
   $form['wtf']=array(
      '#type' => 'value',
      '#value' => $wtf,
   );
   //$result=db_query('SELECT type_of_exception, daynumber, dateofexcept, time_start, time_stop FROM {onlinerecord_exceptions}
   //WHERE plan_id=%d and except_id=%d', $arg, $exception_id);
   //$except=db_fetch_object($result);

   $types_of_exception=array();
   $types_of_exception[0]='---';
   $types_of_exception[1]='everyday';
   $types_of_exception[2]='everyweek';
   $types_of_exception[3]='everymonth';
   $types_of_exception[4]='everyyear';
   $types_of_exception[5]='custom';

   $form['type_of_exception'] = array(
      '#title' => t('Type of exception'),
      '#type' => 'select',
      '#options' => $types_of_exception,
      '#default_value' => $except->type,
   );

   //Set number of quants
   $n=($wtf-$wts)/($kvant_value*60);

   $kvantn=array();
   $kvantn[-1]="---";

   for ($i=0;$i<$n;$i++)
   {
     $temp=$wts+$i*$kvant_value*60;
     $kvantn[$i]=strval((int)($temp/3600)).":".//Hours
       str_pad(strval((int)(($temp%3600)/60)),2,'0',STR_PAD_LEFT);     //minutes with leading NULLs;
   }
   
   $ks=($except->time_start-$wts)/(60*$kvant_value);
   $kf=($except->time_stop-$wts)/(60*$kvant_value);

   $form['kvant_start'] = array(
     '#title' => t('Start quant'),
     '#type' => 'select',
     '#options' => $kvantn,
     '#default_value' => $ks,
     );
   
   $form['kvant_finish'] = array(
     '#title' => t('Finish quant'),
     '#type' => 'select',
     '#options' => $kvantn,
     '#default_value' => $kf,
     );
   //if type=2 (everyweek) or 3 (everymonth)
   //then we need a daynumber
   $form['daynumber'] = array(
      '#title' => t('Daynumber'),
      '#type' => 'select',
      '#options' => array(0=>' ',1=>'1',2=>'2',3=>'3',4=>'4',5=>'5',6=>'6',7=>'7'),
      '#default_value' => $except->daynumber,
   );// select?


   $dateformat='Y-m-d';
   $datef=date($dateformat);

   //if type=4 (everyyear) or 5 (custom)
   //then we need a daynumber
   $form['dateofexception'] = array(
      '#title' => t('Date of Exception'),
      '#type' => 'date_select',
      '#default_value' => $except->dateofexcept,
      '#date_format' => $dateformat,
      '#date_timezone' => 'Asia/Novosibirsk',
      '#date_increment' => $kvant_value,
      '#date_year_range' => '0:+1',
   );

   $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );

   return $form;
}
function onlinerecord_exceptions_edit_validate($form, &$form_state)
{
    $plan_id=intval($form_state['values']['plan_id']);
    $kv=intval($form_state['values']['kvant_value']);
    $wts=intval($form_state['values']['wts']);
    $wtf=intval($form_state['values']['wtf']);

    $type_of_exception =intval($form_state['values']['type_of_exception']);
    $kvant1=intval($form_state['values']['kvant_start']);
    $kvant2=intval($form_state['values']['kvant_finish']);

    $dn=intval($form_state['values']['daynumber']);
    $dateofe=$form_state['values']['dateofexcept'];

    if ($kvant2<$kvant1)
      form_set_error('kvant_start',t('kvant_start > kvant_finish'));

    if ((($type_of_exception)==2)&&($daynumber==0) )
    {
          form_set_error('daynumber',t('Daynumber must be setted correctly/'));
    };

    if ((($type_of_exception)==5)&&($dateofe=='') )
    {
          form_set_error('dateofexcept',t('Date Of Exception must be setted correctly/'));
    };


}


function onlinerecord_exceptions_edit_submit($form, &$form_state)
{
    $except_id=intval($form_state['values']['except_id']);
    $kv=intval($form_state['values']['kvant_value']);
    $wts=intval($form_state['values']['wts']);
    $wtf=intval($form_state['values']['wtf']);

    $type_of_exception=intval($form_state['values']['type_of_exception']);
    $kvant1=intval($form_state['values']['kvant_start']);
    $kvant2=intval($form_state['values']['kvant_finish']);

    $dn=intval($form_state['values']['daynumber']);
    $dateofe=$form_state['values']['dateofexcept'];

    switch($type_of_exception) {
      case 0: //error - type not selected
        break;
      case 1://everyday
        $result=db_query("UPDATE {onlinerecord_exceptions}
          set type=%d,
            time_start=%d,
            time_stop=%d 
          WHERE except_id=%d",
            $type_of_exception,
            $wts+$kvant1*$kv*60,// Start time
            $wts+$kvant2*$kv*60,
            $except_id);// Finish time
        break;
      case 2://everyweek we need correct daynumber (1-Monday, 7-Sunday)
        $result=db_query("UPDATE {onlinerecord_exceptions}
          set type=%d,
            time_start=%d,
            time_stop=%d
            daynumber=%d
          WHERE except_id=%d",$type_of_exception,
            $wts+$kvant1*$kv*60,
            $wts+$kvant2*$kv*60,
            $dn,
            $except_id);
        break;
      case 3: // @TODO everymonth
        break;
      case 4: // @TODO everyyear
        break;
      case 5://csutom we need correct dateofexception )
        $result=db_query("UPDATE {onlinerecord_exceptions}
          set type=%d,
          time_start=%d,
          time_stop=%d,
          dateofexcept='%s'
          WHERE except_id=%d",
            $type_of_exception,
            $wts+$kvant1*$kv*60,// Start time
            $wts+$kvant2*$kv*60,// Finish time
            $dateofe,
            $except_id);
        break;
    }

    drupal_set_message('result='.$result);
}


function onlinerecord_exceptions_form_delete_confirm(&$form_state)
{
    $message=t('Are you sure want to delete exception?');
    $arg=check_plain(arg(4));

    $result=db_query("Select plan_id FROM {onlinerecord_exceptions} WHERE except_id=%d",$arg);

    $plan = db_fetch_object($result);

    $form['except_id']=array(
      '#type' => 'value',
      '#value' => $arg,
    );
    $form['plan_id']=array(
      '#type' => 'value',
      '#value' => $plan,
    );
    
    //drupal_set_message('array orgs=<PRE>'.(print_r($GLOBALS,1)).'<PRE>', 'error');
    return confirm_form(
      $form,
      $message,
      'admin/settings/onlinerecord/orgs',
      '<i>this operation could not be canceled!</i>',
      'Delete',
      'Cancel'
    );
}

function onlinerecord_exceptions_form_delete_confirm_submit($form, &$form_state)
{
    if ($form_state['values']['confirm']) {
      $except_id=$form_state['values']['except_id'];
      
      db_query("DELETE FROM {onlinerecord_exceptions} WHERE except_id=%d",$except_id);
      drupal_set_message('The exception #<i>'.$except_id.'</i> was deleted','status');
      $form_state['redirect']='admin/settings/onlinerecord/plans/'.strval($plan).'exceptions/';
      //@TODO - Department, plan, and records in online_record controls
    }
}


////////////////////

function onlinerecord_exceptions_per_plan_list(&$form_state)
{
    $arg=check_plain(arg(4));
    //@TODO - New field DisplayName or innerjoin?
    $result=db_query('SELECT e.except_id, p.name as pname,
    d.name as dname,
    o.name AS oname ,
    e.type, e.time_start, e.time_stop, e.daynumber, e.dateofexcept
    FROM {onlinerecord_exceptions} e,  {onlinerecord_plans} p, {onlinerecord_organizations} o, {onlinerecord_departments} d
    WHERE p.plan_id=%d
    AND p.dept_id=d.dept_id
    AND d.org_id=o.org_id',$arg);

    $tableHeader = array('plan_name', 'deptname','type', 'time_start', 'time_stop', 'daynumber', 'dateofexcept', 'actions');

    $tableData = array();

    while ($plan = db_fetch_object($result))
    {
        $actions  = l('изменить', 'admin/settings/onlinerecord/exceptions/'.$plan->except_id .'/edit/', array('alias' => true)) . ' | ';
        $actions .= l('удалить', 'admin/settings/onlinerecord/exceptions/'.$plan->except_id.'/delete/', array('alias' => true));

        $tableData[] = array(
            $plan->pname,
            $plan->dname." ".$plan->oname,
            $plan->type,//@TODO Readeable typename
            $plan->kvant_start,
            $plan->kvant_stop,
            $plan->daynumber,
            $plan->dateofexcept,
            $actions,
        );
    }

    return theme('table', $tableHeader, $tableData);
    //return $form;
}

function onlinerecord_exceptions_list(&$form_state)
{
    $arg=check_plain(arg(4));

    //@TODO - New field DisplayName or innerjoin?
    $result=db_query('SELECT e.except_id, p.name as pname,
    d.name as dname,
    o.name AS oname ,
    e.type, e.time_start, e.time_stop, e.daynumber, e.dateofexcept
    FROM {onlinerecord_exceptions} e,  {onlinerecord_plans} p, {onlinerecord_organizations} o, {onlinerecord_departments} d
    WHERE p.dept_id=d.dept_id
    AND d.org_id=o.org_id',$arg);

    $tableHeader = array('plan_name', 'deptname','type', 'time_start', 'time_stop', 'daynumber', 'dateofexcept', 'actions');

    $tableData = array();

    while ($plan = db_fetch_object($result))
    {
        $actions  = l('изменить', 'admin/settings/onlinerecord/exceptions/'.$plan->except_id .'/edit/', array('alias' => true)) . ' | ';
        $actions .= l('удалить', 'admin/settings/onlinerecord/exceptions/'.$plan->except_id.'/delete/', array('alias' => true));

        $tableData[] = array(
            $plan->pname,
            $plan->dname." ".$plan->oname,
            $plan->type,//@TODO Readeable typename
            $plan->kvant_start,
            $plan->kvant_stop,
            $plan->daynumber,
            $plan->dateofexcept,
            $actions,
        );
    }

    return theme('table', $tableHeader, $tableData);
}



?>