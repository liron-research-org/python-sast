require "abstract_unit"
require "controller/fake_controllers"
require "ssrf_filter"

class ActionPackAssertionsController < ActionController::Base
  def nothing() head :ok end
 as
  # ok: tainted-url-host
  def hello_xml_world() render template: "test/hello_xml_world"; end 

  def assign_this
    @howdy = "ho"
    # ok: tainted-url-host
    render inline: "Mr. Henke"
  end

  def render_based_on_parameters
    # ok: tainted-url-host
    render plain: "Mr. #{params[:name]}"
  end

  def render1
    # ruleid: tainted-url-host
    render inline: "<a href='http://#{params[:name]}/path'></div>"
  end

  
  def sanitized
    # ok: tainted-url-host
    response = SsrfFilter.get(params[:url])
    response
  end
end


class UsersController < ApplicationController
  skip_before_action :has_info
  skip_before_action :authenticated, only: [:new, :create]

  def new
    @user = User.new
  end


  def update1
    message = false
    # ruleid:tainted-sql-string
    user = User.find(:first, :conditions => "user_id = '#{params[:user][:user_id]}'")
    user.skip_user_id_assign = true
    user.update_attributes(params[:user].reject { |k| k == ("password" || "password_confirmation") || "user_id" })
    pass = params[:user][:password]
    user.password = pass if !(pass.blank?)
    message = true if user.save!
    respond_to do |format|
      format.html { redirect_to user_account_settings_path(:user_id => current_user.user_id) }
      format.json { render :json => {:msg => message ? "success" : "false "} }
    end
  end

  def ok_test1
    # ok:tainted-sql-string
    message = "this is just a message ! %s" % params[:user]
    redirect_to '/'
  end

  def ok_test2
    # ok:tainted-sql-string
    message = Kernel::sprintf("this message is ok: '%s'", params[:user])
    redirect_to '/'
  end

  def ok_test3
    # ok:tainted-sql-string
    records = "this is ok!" + params[:user] + "'"
    redirect_to '/'
  end

end
