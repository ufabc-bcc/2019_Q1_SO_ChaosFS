use std::ffi::CStr;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn cmp_nomes_rs(a: &CStr, b: &CStr) -> bool {
    let str_a = match a.to_str() {
        Ok(s) => s,
        _ => return false,
    };
    let str_b = match b.to_str() {
        Ok(s) => s,
        _ => return false,
    };
    let split_a: Vec<&str> = str_a.split('/').collect();
    let split_b: Vec<&str> = str_b.split('/').collect();
    let ma = match split_a.last() {
        Some(s) => s,
        None => "",
    };
    let mb = match split_b.last() {
        Some(s) => s,
        None => "",
    };
    ma == mb
}

// #[no_mangle]
// pub extern "C" fn last_name(a: &CStr) -> *const c_char {
//     let str_a = match a.to_str() {
//         Ok(s) => s,
//         _ => ""
//     };
//     let split_a: Vec<&str> = str_a.split('/').collect();
//     let ma = match split_a.last() {
//         Some(s) => s,
//         None => "",
//     };
//     ma.as_ptr() as *const c_char
// }
