! wrapfClass2.f
! This is generated code, do not edit
!>
!! \file wrapfClass2.f
!! \brief Shroud generated wrapper for Class2 class
!<
module class2_mod
    use iso_c_binding, only : C_PTR
    ! splicer begin class.Class2.module_use
    ! splicer end class.Class2.module_use
    implicit none
    
    
    ! splicer begin class.Class2.module_top
    ! splicer end class.Class2.module_top
    
    type class2
        type(C_PTR), private :: voidptr
        ! splicer begin class.Class2.component_part
        ! splicer end class.Class2.component_part
    contains
        procedure :: method1 => class2_method1
        procedure :: get_instance => class2_get_instance
        procedure :: set_instance => class2_set_instance
        procedure :: associated => class2_associated
        ! splicer begin class.Class2.type_bound_procedure_part
        ! splicer end class.Class2.type_bound_procedure_part
    end type class2
    
    
    interface operator (.eq.)
        module procedure class2_eq
    end interface
    
    interface operator (.ne.)
        module procedure class2_ne
    end interface
    
    interface
        
        subroutine c_class2_method1(self) &
                bind(C, name="DEF_class2_method1")
            use iso_c_binding
            implicit none
            type(C_PTR), value, intent(IN) :: self
        end subroutine c_class2_method1
        
        ! splicer begin class.Class2.additional_interfaces
        ! splicer end class.Class2.additional_interfaces
    end interface

contains
    
    subroutine class2_method1(obj)
        implicit none
        class(class2) :: obj
        ! splicer begin class.Class2.method.method1
        call c_class2_method1(obj%voidptr)
        ! splicer end class.Class2.method.method1
    end subroutine class2_method1
    
    function class2_get_instance(obj) result (voidptr)
        use iso_c_binding, only: C_PTR
        implicit none
        class(class2), intent(IN) :: obj
        type(C_PTR) :: voidptr
        voidptr = obj%voidptr
    end function class2_get_instance
    
    subroutine class2_set_instance(obj, voidptr)
        use iso_c_binding, only: C_PTR
        implicit none
        class(class2), intent(INOUT) :: obj
        type(C_PTR), intent(IN) :: voidptr
        obj%voidptr = voidptr
    end subroutine class2_set_instance
    
    function class2_associated(obj) result (rv)
        use iso_c_binding, only: c_associated
        implicit none
        class(class2), intent(IN) :: obj
        logical rv
        rv = c_associated(obj%voidptr)
    end function class2_associated
    
    ! splicer begin class.Class2.additional_functions
    ! splicer end class.Class2.additional_functions
    
    function class2_eq(a,b) result (rv)
        use iso_c_binding, only: c_associated
        implicit none
        type(class2), intent(IN) ::a,b
        logical :: rv
        if (c_associated(a%voidptr, b%voidptr)) then
            rv = .true.
        else
            rv = .false.
        endif
    end function class2_eq
    
    function class2_ne(a,b) result (rv)
        use iso_c_binding, only: c_associated
        implicit none
        type(class2), intent(IN) ::a,b
        logical :: rv
        if (.not. c_associated(a%voidptr, b%voidptr)) then
            rv = .true.
        else
            rv = .false.
        endif
    end function class2_ne

end module class2_mod
